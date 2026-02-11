#include "physics/henPhysics.h"

#include "vendor/JoltPhysics/Jolt/Jolt.h"
#include "vendor/JoltPhysics/Jolt/RegisterTypes.h"
#include "vendor/JoltPhysics/Jolt/Core/Factory.h"
#include "vendor/JoltPhysics/Jolt/Core/TempAllocator.h"
#include "vendor/JoltPhysics/Jolt/Core/JobSystemThreadPool.h"
#include "vendor/JoltPhysics/Jolt/Physics/PhysicsSettings.h"
#include "vendor/JoltPhysics/Jolt/Physics/PhysicsSystem.h"
#include "vendor/JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.h"
#include "vendor/JoltPhysics/Jolt/Physics/Body/BodyActivationListener.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/CylinderShape.h"

JPH_SUPPRESS_WARNINGS

#include "core/henTimer.h"
#include "core/henMath.h"
#include "core/henCVar.h"
#include "core/henJobSystem.h"
#include "level/henLevel.h"
#include "tools/henConsole.h"

#include <thread>

using namespace JPH::literals;

namespace hen::physics
{
	bool Initialised = false;

	cvar::CVar cvar_SimulationEnabled("phys_sim_enabled", true, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_InterpolationEnabled("phys_sim_interpolate", true, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_Accuracy("phys_sim_accuracy", 4, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_HZ("phys_sim_hz", 60, cvar::FLAGS_ARCHIVE);

	namespace jolt
	{
		using namespace JPH;
		using namespace JPH::literals;

		static const uint MaxBodies = 65536;
		static const uint NumberBodyMutexes = 0;
		static const uint MaxBodyPairs = 65536;
		static const uint MaxContactConstraints = 65536;
		const EMotionQuality MotionQuality = EMotionQuality::LinearCast;

		// dont really like these namespaces acting as enums but it is what it is
		namespace LAYERS
    	{
    	    static constexpr ObjectLayer STATIC = 0;
    	    static constexpr ObjectLayer DYNAMIC = 1;
    	    static constexpr ObjectLayer NUM_LAYERS = 2;
    	}
	
		// right now im using a 1 to 1 mapping however i could change that in the future
		// if i do that then JPH_TRACK_BROADPHASE_STATS is important
    	namespace BROADPHASELAYERS
    	{
    		static constexpr BroadPhaseLayer STATIC(0);
    		static constexpr BroadPhaseLayer DYNAMIC(1);
    		static constexpr uint NUM_LAYERS(2);
    	};

		class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
    	{
    	public:
    		bool ShouldCollide(ObjectLayer object1, ObjectLayer object2) const override
    		{
    			switch (object1)
    			{
    			case LAYERS::STATIC:
    				return object2 == LAYERS::DYNAMIC; // static only collides with dynamic
    			case LAYERS::DYNAMIC:
    				return true;
    			default:
    				HEN_ASSERT(false, "Object(s) have invalid layer");
    				return false;
    			}
    		}
    	};

		class ObjectBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    	{
    	public:
    		bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override
    		{
    			switch (layer1)
    			{
    			case LAYERS::STATIC:
    				return layer2 == BROADPHASELAYERS::STATIC;
    			case LAYERS::DYNAMIC:
    				return true;
    			default:
    				HEN_ASSERT(false, "Object(s) have invalid layer");
    				return false;
    			}
    		}
    	};

		class BroadPhaseLayerInterfaceImpl final : public BroadPhaseLayerInterface
		{
		public:
			BroadPhaseLayerInterfaceImpl()
			{
				// 1 to 1 mapping
				m_ObjectToBroadPhase[LAYERS::STATIC] = BROADPHASELAYERS::STATIC;
				m_ObjectToBroadPhase[LAYERS::DYNAMIC] = BROADPHASELAYERS::DYNAMIC;
			}

			uint GetNumBroadPhaseLayers() const override
			{
				return BROADPHASELAYERS::NUM_LAYERS;
			}

			BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer layer) const override
			{
				HEN_ASSERT(layer < LAYERS::NUM_LAYERS, "Invalid layer passed");
				return m_ObjectToBroadPhase[layer];
			}

			#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    			const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override
    			{
    				switch ((JPH::BroadPhaseLayer::Type)layer)
    				{
    				    case (JPH::BroadPhaseLayer::Type)BROADPHASELAYERS::STATIC:	
        		            return "STATIC";
    				    case (JPH::BroadPhaseLayer::Type)BROADPHASELAYERS::DYNAMIC:		
        		            return "DYNAMIC";
    				    default:													
        		            HEN_ASSERT(false, "Broadphase is in an invalid layer"); 
							return "INVALID";
    				}
    			}
    		#endif // !JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

		private:
			BroadPhaseLayer m_ObjectToBroadPhase[LAYERS::NUM_LAYERS];
		};

		struct PhysicsLevel
		{
			PhysicsSystem System;
			BroadPhaseLayerInterfaceImpl BroadPhaseLayerInterface;
			ObjectLayerPairFilterImpl ObjectObjectLayerFilter;
			ObjectBroadPhaseLayerFilterImpl ObjectBroadPhaseLayerFilter;

			float Accumulator;
			float Alpha = 0;

			float GetKinematicDT(float deltaTime) const
			{
				return math::Clamp(Accumulator + deltaTime, 0.0f, cvar_HZ.GetInt() * cvar_Accuracy.GetInt());
			}
		};

		struct RigidBody
		{
			std::shared_ptr<void> ParentPhysicsLevel;
			ShapeRefC Shape;
			BodyID BodyHandle;
			level::Entity Entity;

			Vec3 PreviousPosition = Vec3::sZero();
			Quat PreviousRotation = Quat::sIdentity();

			Mat44 Offset = Mat44::sIdentity();
			Mat44 OffsetInverse = Mat44::sIdentity();

			~RigidBody()
			{
				if (ParentPhysicsLevel == nullptr || BodyHandle.IsInvalid())
				{
					return;
				}

				BodyInterface& interface = ((PhysicsLevel*)ParentPhysicsLevel.get())->System.GetBodyInterface();
				interface.RemoveBody(BodyHandle);
				interface.DestroyBody(BodyHandle);
			}
		};

		// Conversion shit for vectors and stuff

		inline Vec3 cast(const Float3& v) 
		{ 
			return Vec3(v.x, v.y, v.z); 
		}
		inline Vec3 cast(const math::Vec3& v) 
		{ 
			return Vec3(v.x, v.y, v.z); 
		}
		inline math::Vec3 cast(Vec3Arg v) 
		{ 
			return  math::Vec3(v.GetX(), v.GetY(), v.GetZ()); 
		}

		inline Quat cast(const math::Quat& q) 
		{ 
			return Quat(q.x, q.y, q.z, q.w); 
		}
		inline  math::Quat cast(QuatArg q) 
		{ 
			return math::Quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); 
		}

		inline Mat44 cast(const math::Matrix4& m)
		{
			return Mat44( // NOTE: THIS IS GLM SPECIFIC, SO IF ANOTHER MATH LIBRARY IS IMPLEMENTED IN THE FUTURE, THIS WILL SHIT ITSELF
				Vec4(m[0][0], m[0][1], m[0][2], m[0][3]),
				Vec4(m[1][0], m[1][1], m[1][2], m[1][3]),
				Vec4(m[2][0], m[2][1], m[2][2], m[2][3]),
				Vec4(m[3][0], m[3][1], m[3][2], m[3][3])
			);
		}
		inline glm::mat4 cast(const JPH::Mat44& m)
		{
		   glm::mat4 out;
		
		   for (int column = 0; column < 4; ++column)
		   {
		       JPH::Vec4 col = m.GetColumn4(column);

		       out[column][0] = col.GetX();
		       out[column][1] = col.GetY();
		       out[column][2] = col.GetZ();
		       out[column][3] = col.GetW();
		   }
	   
		   return out;
		}

		PhysicsLevel& GetPhysicsLevel(level::Level& level)
		{
			if (level.PhysicsLevel == nullptr)
			{
				auto physLevel = std::make_shared<PhysicsLevel>();

				physLevel->System.Init(MaxBodies, NumberBodyMutexes, MaxBodyPairs, MaxContactConstraints, physLevel->BroadPhaseLayerInterface, physLevel->ObjectBroadPhaseLayerFilter, physLevel->ObjectObjectLayerFilter);

				level.PhysicsLevel = physLevel;
			}

			return *(PhysicsLevel*)level.PhysicsLevel.get();
		}

		RigidBody& GetRigidBody(level::RigidBodyComponent& physComponent)
		{
			if (physComponent.PhysicsObject == nullptr)
			{
				physComponent.PhysicsObject = std::make_shared<RigidBody>();
			}

			return *(RigidBody*)physComponent.PhysicsObject.get();
		}

		void AddRigidBody(level::Level& level, level::Entity entity)
		{
			ShapeSettings::ShapeResult shapeResult;

			float convexRadius = 0.001f;

			if (!entity.HasComponent<level::RigidBodyComponent>())
			{
				HEN_WARN("[hen::physics] Entity " + entity.GetComponent<level::NameComponent>().Name + "doesn't have a rigidbody component");
				return;
			}

			if (!entity.HasComponent<level::TransformComponent>())
			{
				HEN_WARN("[hen::physics] Entity " + entity.GetComponent<level::NameComponent>().Name + "doesn't have a transform component");
				return;
			}

			auto& transformComp = entity.GetComponent<level::TransformComponent>();
			auto& rigidBodyComp = entity.GetComponent<level::RigidBodyComponent>();

			switch (rigidBodyComp.Shape)
			{
				case level::RigidBodyComponent::COLLISIONSHAPES::BOX:
				{
					BoxShapeSettings settings(Vec3(rigidBodyComp.Box.HalfExtents.x * transformComp.LocalScale.x, rigidBodyComp.Box.HalfExtents.y * transformComp.LocalScale.y, rigidBodyComp.Box.HalfExtents.z * transformComp.LocalScale.z));
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
				case level::RigidBodyComponent::COLLISIONSHAPES::SPHERE:
				{
					SphereShapeSettings settings(rigidBodyComp.Sphere.Radius * transformComp.LocalScale.x);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}	
				case level::RigidBodyComponent::COLLISIONSHAPES::CAPSULE:
				{
					CapsuleShapeSettings settings(rigidBodyComp.Capsule.Height * transformComp.LocalScale.y, rigidBodyComp.Capsule.Radius * transformComp.LocalScale.x);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
				case level::RigidBodyComponent::COLLISIONSHAPES::CYLINDER:
				{
					CylinderShapeSettings settings(rigidBodyComp.Cylinder.Height * transformComp.LocalScale.y, rigidBodyComp.Cylinder.Radius * transformComp.LocalScale.x, convexRadius);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
			}

			if (!shapeResult.IsValid())
			{
				std::string error = shapeResult.GetError().c_str();
				HEN_ERROR("[hen::physics] Rigidbody creation failed: " + error);
				return;
			}

			RigidBody& physicsObject = GetRigidBody(rigidBodyComp);
			physicsObject.Shape = shapeResult.Get();
			physicsObject.ParentPhysicsLevel = level.PhysicsLevel;
			physicsObject.Entity = entity;

			PhysicsLevel& physicsLevel = GetPhysicsLevel(level);
			
			Mat44 worldMat = cast(transformComp.GetWorldMatrix());
			Vec3 offset = cast(rigidBodyComp.Offset);

			physicsObject.Offset.SetTranslation(offset);
			physicsObject.OffsetInverse = physicsObject.Offset.Inversed();
			physicsObject.PreviousPosition = worldMat.GetTranslation();
			physicsObject.PreviousRotation = worldMat.GetQuaternion().Normalized();

			const EMotionType motionType = rigidBodyComp.Mass == 0 ? EMotionType::Static : (rigidBodyComp.Kinematic ? EMotionType::Kinematic : EMotionType::Dynamic);

			BodyCreationSettings settings(physicsObject.Shape.GetPtr(), offset + physicsObject.PreviousPosition, physicsObject.PreviousRotation, motionType, LAYERS::DYNAMIC);

			settings.mMassPropertiesOverride.mMass = motionType == EMotionType::Dynamic ? rigidBodyComp.Mass : 1;
			settings.mFriction = rigidBodyComp.Friction;
			settings.mRestitution = rigidBodyComp.Restitution;
			settings.mLinearDamping = rigidBodyComp.LinearDamping;
			settings.mAngularDamping = rigidBodyComp.AngularDamping;
			settings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
			settings.mAllowSleeping = !rigidBodyComp.DisableDeactivation;
			settings.mMotionQuality = MotionQuality;
			settings.mUserData = (uint64_t)&physicsObject;
			
			BodyInterface& interface = physicsLevel.System.GetBodyInterface();

			const EActivation activation = rigidBodyComp.StartDeactivated ? EActivation::DontActivate : EActivation::Activate;

			physicsObject.BodyHandle = interface.CreateAndAddBody(settings, activation);

			if (physicsObject.BodyHandle.IsInvalid())
			{
				HEN_ERROR("[hen::physics] Failed to create rigidbody");
				return;
			}
		}

	}

    void Initialise()
    {
        hen::Timer timer;

		JPH::RegisterDefaultAllocator();
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		Initialised = true;

        HEN_LOG("[hen::physics] Initialised with Jolt Physics " + std::to_string(JPH_VERSION_MAJOR) + "." + std::to_string(JPH_VERSION_MINOR) + "." + std::to_string(JPH_VERSION_PATCH) + " in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Update(float deltaTime)
    {	
		if (!level::GetActiveLevel())
		{
			return;
		}

		if (deltaTime <= 0 || !Initialised)
		{
			return;
		}

		level::Level& currentLevel = *level::GetActiveLevel();
		jolt::PhysicsLevel& physLevel = jolt::GetPhysicsLevel(currentLevel);

		physLevel.System.SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

		auto rbView = currentLevel.GetView<level::RigidBodyComponent>();

		jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel, &currentLevel, &deltaTime](jobsystem::DispatchArgs args)
		{
			auto entity = rbView[args.JobIndex];
			
			if (!entity.HasComponent<level::TransformComponent>())
			{
				return;
			}

			auto& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
			auto& transformComponent = entity.GetComponent<level::TransformComponent>();

			if (rbComponent.PhysicsObject == nullptr)
			{
				jolt::AddRigidBody(currentLevel, entity);
			}
			else
			{
				jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

				if (physObj.BodyHandle.IsInvalid())
				{
					return;
				}

				JPH::BodyInterface& bodyInterface = physLevel.System.GetBodyInterface();
				bodyInterface.SetFriction(physObj.BodyHandle, rbComponent.Friction);
				bodyInterface.SetRestitution(physObj.BodyHandle, rbComponent.Restitution);
				const JPH::EMotionType prevMotionType = bodyInterface.GetMotionType(physObj.BodyHandle);
				const JPH::EMotionType currentMotionType = rbComponent.Mass == 0 ? JPH::EMotionType::Static : (rbComponent.Kinematic ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic);

				if (prevMotionType != currentMotionType)
				{
					bodyInterface.SetMotionType(physObj.BodyHandle, currentMotionType, JPH::EActivation::Activate);
				}

				bodyInterface.ActivateBody(physObj.BodyHandle);

				const JPH::Vec3 position = jolt::cast(transformComponent.LocalPosition);
				const JPH::Quat rotation = jolt::cast(transformComponent.LocalRotation);
			
				JPH::Mat44 matrix = JPH::Mat44::sTranslation(position) * JPH::Mat44::sRotation(rotation);
				matrix = matrix * physObj.Offset;

				if (cvar_SimulationEnabled.GetBool())
				{
					if (currentMotionType == JPH::EMotionType::Kinematic)
					{
						bodyInterface.MoveKinematic(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), physLevel.GetKinematicDT(deltaTime));
					}
					else if (currentMotionType == JPH::EMotionType::Static || !bodyInterface.IsActive(physObj.BodyHandle))
					{
						bodyInterface.SetPositionAndRotation(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), JPH::EActivation::DontActivate);
					}
				}
				else
				{
					physObj.PreviousPosition = position;
					physObj.PreviousRotation = rotation;
					bodyInterface.SetPositionAndRotation(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), JPH::EActivation::Activate);
				}
			}
		});

		if (cvar_SimulationEnabled.GetBool())
    	{
    	    static JPH::TempAllocatorMalloc tempAllocator;
    	    static JPH::JobSystemThreadPool joltJobSystem(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    	    const float fixedStep = 1.0f / cvar_HZ.GetInt();

    	    physLevel.Accumulator += deltaTime;
    	    physLevel.Accumulator = math::Clamp(physLevel.Accumulator, 0.0f, fixedStep * cvar_Accuracy.GetInt());

    	    while (physLevel.Accumulator >= fixedStep)
    	    {
    	        if (cvar_InterpolationEnabled.GetBool())
    	        {
    	            jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel](jobsystem::DispatchArgs args)
    	            {
    	                auto entity = rbView[args.JobIndex];
    	                auto& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
    	                jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

    	                if (physObj.BodyHandle.IsInvalid())
    	                    return;

    	                JPH::BodyInterface& bodyInterface = physLevel.System.GetBodyInterfaceNoLock();
    	                JPH::Mat44 matrix = bodyInterface.GetWorldTransform(physObj.BodyHandle);
    	                matrix = matrix * physObj.OffsetInverse;

    	                physObj.PreviousPosition = matrix.GetTranslation();
    	                physObj.PreviousRotation = matrix.GetQuaternion().Normalized();
    	            });
    	        }

    	        physLevel.System.Update(fixedStep, 1, &tempAllocator, &joltJobSystem);
    	        physLevel.Accumulator -= fixedStep;
    	    }

    	    physLevel.Alpha = math::Clamp(physLevel.Accumulator / fixedStep, 0.0f, 1.0f);
    	}


		jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel, &currentLevel](jobsystem::DispatchArgs args) 
		{
			auto entity = rbView[args.JobIndex];

			if (!entity.HasComponent<level::TransformComponent>())
			{
				return;
			}

			auto& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
			auto& transformComponent = entity.GetComponent<level::TransformComponent>();
			
			jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

			if (physObj.BodyHandle.IsInvalid())
			{
				return;
			}

			JPH::BodyInterface& bodyInterface = physLevel.System.GetBodyInterfaceNoLock();

			if (bodyInterface.GetMotionType(physObj.BodyHandle) != JPH::EMotionType::Dynamic)
			{
				return;
			}

			JPH::Mat44 matrix = bodyInterface.GetWorldTransform(physObj.BodyHandle);
			matrix = matrix * physObj.OffsetInverse;

			JPH::Vec3 position = matrix.GetTranslation();
			JPH::Quat rotation = matrix.GetQuaternion().Normalized();

			if (cvar_InterpolationEnabled.GetBool())
			{
				position = position * physLevel.Alpha + physObj.PreviousPosition * (1 - physLevel.Alpha);
				rotation = physObj.PreviousRotation.SLERP(rotation, physLevel.Alpha);
			}

			transformComponent.SetLocalPosition(jolt::cast(position));
			transformComponent.SetLocalRotation(jolt::cast(rotation));
		});

		jobsystem::Wait();

    }

	void Shutdown()
	{
		JPH::UnregisterTypes();
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}
}