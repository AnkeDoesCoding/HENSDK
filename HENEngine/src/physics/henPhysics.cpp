#include "physics/henPhysics.h"

#include "core/henTimer.h"
#include "core/henMath.h"
#include "core/henCVar.h"
#include "core/henJobSystem.h"
#include "level/henLevel.h"
#include "tools/henConsole.h"

#include <thread>

#include <JoltPhysics/Jolt/Jolt.h>
#include <JoltPhysics/Jolt/RegisterTypes.h>
#include <JoltPhysics/Jolt/Core/Factory.h>
#include <JoltPhysics/Jolt/Core/TempAllocator.h>
#include <JoltPhysics/Jolt/Core/JobSystemThreadPool.h>
#include <JoltPhysics/Jolt/Physics/PhysicsSettings.h>
#include <JoltPhysics/Jolt/Physics/PhysicsSystem.h>
#include <JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/MeshShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <JoltPhysics/Jolt/Physics/Collision/RayCast.h>
#include <JoltPhysics/Jolt/Physics/Collision/CastResult.h>
#include <JoltPhysics/Jolt/Physics/Collision/CollisionCollectorImpl.h>

JPH_SUPPRESS_WARNINGS

using namespace JPH::literals;

namespace hen::physics
{
	bool Initialised = false;

	cvar::CVar cvar_SimulationEnabled("phys_sim_enabled", true, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_InterpolationEnabled("phys_sim_interpolate", true, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_Accuracy("phys_sim_accuracy", 4, cvar::FLAGS_ARCHIVE);
	cvar::CVar cvar_HZ("phys_sim_hz", 60, cvar::FLAGS_ARCHIVE);

	// jolt and hen's coordinate systems are both right handed y up

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
    				switch (static_cast<JPH::BroadPhaseLayer::Type>(layer))
    				{
    				    case static_cast<JPH::BroadPhaseLayer::Type>(BROADPHASELAYERS::STATIC):	
        		            return "STATIC";
    				    case static_cast<JPH::BroadPhaseLayer::Type>(BROADPHASELAYERS::DYNAMIC):		
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

		class JobSystemImpl : public JPH::JobSystem
		{
		public:
			int GetMaxConcurrency() const override
			{
				return std::thread::hardware_concurrency() - 1;
			}

			JobHandle CreateJob(const char* name, JPH::ColorArg colour, const JobFunction& jobFunction, uint32_t numDependencies = 0) override
			{
				Job* job = new Job(name, colour, this, jobFunction, numDependencies);
				job->AddRef();

				if (numDependencies == 0)
				{
					QueueJob(job);
				}

				return JobHandle(job);
			}

			Barrier* CreateBarrier() override
			{
				return new BarrierImpl();
			}

			void DestroyBarrier(Barrier* barrier) override
			{
				BarrierImpl* cast = static_cast<BarrierImpl*>(barrier);
				delete cast;
			}

			void WaitForJobs(Barrier* barrier) override
			{
				BarrierImpl* cast = static_cast<BarrierImpl*>(barrier);
				cast->Wait();
			}

		protected:
			void QueueJob(Job* job) override
			{
				job->AddRef();

				hen::jobsystem::Execute([job]()
				{
                	job->Execute();
                	job->Release(); 
				});
			}

			void QueueJobs(Job** jobs, uint numOfJobs) override
			{
				for (uint i = 0; i < numOfJobs; ++i)
				{
					QueueJob(jobs[i]);
				}
			}

			void FreeJob(Job* job) override
			{
				delete job;
			}

		private:
			class BarrierImpl : public JPH::JobSystem::Barrier
			{
			public:
				void AddJob(const JobHandle& jobHandle) override
				{
					std::lock_guard<std::mutex> lock(m_Mutex);

					jobHandle.GetPtr()->AddRef(); 
					m_Jobs.push_back(jobHandle.GetPtr());
				}

				void AddJobs(const JobHandle* handles, uint numOfHandles) override
				{
					std::lock_guard<std::mutex> lock(m_Mutex);

					for (uint i = 0; i < numOfHandles; ++i)
					{
						handles[i].GetPtr()->AddRef();
						m_Jobs.push_back(handles[i].GetPtr());
					}
				}

				void Wait()
				{
					while (true)
					{
						std::vector<Job*> jobsSnapshot;

						{
							std::lock_guard<std::mutex> lock(m_Mutex);

							jobsSnapshot = m_Jobs;
						}

						bool allDone = true;
						for (Job* job : jobsSnapshot)
						{
							if (!job->IsDone())
							{
								allDone = false;
								break;
							}
						}

						if (allDone && jobsSnapshot.empty())
						{
							break;
						}

						{
							std::lock_guard<std::mutex> lock(m_Mutex);
							
							for (auto it = m_Jobs.begin(); it != m_Jobs.end();)
							{
								if ((*it)->IsDone())
								{
									(*it)->Release();
									it = m_Jobs.erase(it);
								}
								else
								{
									++it;
								}
							}
						}

						std::this_thread::yield();
					}
				}

			protected:
				void OnJobFinished(Job* job) override
				{
					std::lock_guard<std::mutex> lock(m_Mutex);

					auto it = std::find(m_Jobs.begin(), m_Jobs.end(), job);
					if (it != m_Jobs.end())
					{
						job->Release();
						m_Jobs.erase(it);
					}
				}

			private:
				std::mutex m_Mutex;
				std::vector<Job*> m_Jobs;
			};
		};

		struct PhysicsLevel
		{
			PhysicsSystem System;
			BroadPhaseLayerInterfaceImpl BroadPhaseLayerInterface;
			ObjectLayerPairFilterImpl ObjectObjectLayerFilter;
			ObjectBroadPhaseLayerFilterImpl ObjectBroadPhaseLayerFilter;

			float Accumulator = 0.0f;
			float Alpha = 0.0f;

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

				BodyInterface& interface = (reinterpret_cast<PhysicsLevel*>(ParentPhysicsLevel.get())->System.GetBodyInterface());
				interface.RemoveBody(BodyHandle);
				interface.DestroyBody(BodyHandle);
			}
		};

		// Conversion shit for vectors and stuff

		inline Vec3 Cast(const math::Vec3& v) 
		{ 
			return Vec3(v.x, v.y, v.z); 
		}
		inline math::Vec3 Cast(Vec3Arg v) 
		{ 
			return  math::Vec3(v.GetX(), v.GetY(), v.GetZ()); 
		}

		inline Quat Cast(const math::Quat& q) 
		{ 
			return Quat(q.x, q.y, q.z, q.w); 
		}
		inline math::Quat Cast(QuatArg q) 
		{ 
			return math::Quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); 
		}

		inline Mat44 Cast(const math::Matrix4& m)
		{
			return Mat44( // NOTE: THIS IS GLM SPECIFIC, SO IF ANOTHER MATH LIBRARY IS IMPLEMENTED IN THE FUTURE, THIS WILL SHIT ITSELF
				Vec4(m[0][0], m[0][1], m[0][2], m[0][3]),
				Vec4(m[1][0], m[1][1], m[1][2], m[1][3]),
				Vec4(m[2][0], m[2][1], m[2][2], m[2][3]),
				Vec4(m[3][0], m[3][1], m[3][2], m[3][3])
			);
		}
		inline glm::mat4 Cast(const JPH::Mat44& m)
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
				std::shared_ptr<PhysicsLevel> physLevel = std::make_shared<PhysicsLevel>();

				physLevel->System.Init(MaxBodies, NumberBodyMutexes, MaxBodyPairs, MaxContactConstraints, physLevel->BroadPhaseLayerInterface, physLevel->ObjectBroadPhaseLayerFilter, physLevel->ObjectObjectLayerFilter);
				physLevel->System.OptimizeBroadPhase();

				level.PhysicsLevel = physLevel;
			}

			return *reinterpret_cast<PhysicsLevel*>(level.PhysicsLevel.get());
		}

		RigidBody& GetRigidBody(level::RigidBodyComponent& physComponent)
		{
			if (physComponent.PhysicsObject == nullptr)
			{
				physComponent.PhysicsObject = std::make_shared<RigidBody>();
			}

			return *reinterpret_cast<RigidBody*>(physComponent.PhysicsObject.get());
		}

		void AddRigidBody(level::Level& level, level::Entity entity)
		{
			ShapeSettings::ShapeResult shapeResult = ShapeSettings::ShapeResult();

			float convexRadius = 0.001f;

			if (!entity.HasComponent<level::RigidBodyComponent>())
			{
				HEN_ERROR("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + "doesn't have a rigidbody component");
				return;
			}

			if (!entity.HasComponent<level::TransformComponent>())
			{
				HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + "doesn't have a transform component");
				return;
			}

			level::TransformComponent& transformComp = entity.GetComponent<level::TransformComponent>();
			level::RigidBodyComponent& rigidBodyComp = entity.GetComponent<level::RigidBodyComponent>();

			switch (rigidBodyComp.Shape)
			{
				case level::COLLISIONSHAPES::BOX:
				{
					BoxShapeSettings settings(Vec3(rigidBodyComp.Box.HalfExtents.x * transformComp.LocalScale.x, rigidBodyComp.Box.HalfExtents.y * transformComp.LocalScale.y, rigidBodyComp.Box.HalfExtents.z * transformComp.LocalScale.z), convexRadius);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
				case level::COLLISIONSHAPES::SPHERE:
				{
					SphereShapeSettings settings(rigidBodyComp.Sphere.Radius * transformComp.LocalScale.x);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}	
				case level::COLLISIONSHAPES::CAPSULE:
				{
					CapsuleShapeSettings settings(rigidBodyComp.Capsule.Height * transformComp.LocalScale.y, rigidBodyComp.Capsule.Radius * transformComp.LocalScale.x);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
				case level::COLLISIONSHAPES::CYLINDER:
				{
					CylinderShapeSettings settings(rigidBodyComp.Cylinder.Height * transformComp.LocalScale.y, rigidBodyComp.Cylinder.Radius * transformComp.LocalScale.x, convexRadius);
					settings.SetEmbedded();
					shapeResult = settings.Create();
					break;
				}
				case level::COLLISIONSHAPES::CONVEX_HULL:
				{
					if (!entity.HasComponent<level::MeshComponent>())
					{
						HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + " requested convex hull collision but doesn't have a mesh component, resolving to box collision");

						BoxShapeSettings settings(Vec3(rigidBodyComp.Box.HalfExtents.x * transformComp.LocalScale.x, rigidBodyComp.Box.HalfExtents.y * transformComp.LocalScale.y, rigidBodyComp.Box.HalfExtents.z * transformComp.LocalScale.z), convexRadius);
						settings.SetEmbedded();
						shapeResult = settings.Create();

						break;
					}

					level::MeshComponent& meshComponent = entity.GetComponent<level::MeshComponent>();

					if (meshComponent.State != graphics::RESOURCE_STATES::READY_TO_RENDER)
					{
						HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + "'s mesh isn't ready to be processed yet, will reattempt soon");
						break;
					}

					Array<Vec3> points;
					points.reserve(meshComponent.Vertices.size());
					for (math::Vec3& pos : meshComponent.Vertices)
					{
						points.push_back(Vec3(pos.x * transformComp.LocalScale.x, pos.y * transformComp.LocalScale.y, pos.z * transformComp.LocalScale.z));
					}

					ConvexHullShapeSettings settings(points, convexRadius);
					settings.SetEmbedded();
					shapeResult = settings.Create();

					break;
				}
				case level::COLLISIONSHAPES::TRIANGLE_MESH:
				{
					if (!entity.HasComponent<level::MeshComponent>())
					{
						HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + " requested triangle mesh collision but doesn't have a mesh component, resolving to box collision");

						BoxShapeSettings settings(Vec3(rigidBodyComp.Box.HalfExtents.x * transformComp.LocalScale.x, rigidBodyComp.Box.HalfExtents.y * transformComp.LocalScale.y, rigidBodyComp.Box.HalfExtents.z * transformComp.LocalScale.z), convexRadius);
						settings.SetEmbedded();
						shapeResult = settings.Create();

						break;
					}

					level::MeshComponent& meshComponent = entity.GetComponent<level::MeshComponent>();

					if (meshComponent.State != graphics::RESOURCE_STATES::READY_TO_RENDER)
					{
						HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + "'s mesh isn't ready to be processed yet, will reattempt soon");
						break;
					}

					TriangleList list;

					for (uint32_t submeshIndex = 0; submeshIndex < meshComponent.SubMeshes.size(); submeshIndex++)
    				{
    				    const level::MeshComponent::SubMesh& submesh = meshComponent.SubMeshes[submeshIndex];
					
    				    HEN_ASSERT(submesh.IndexStart + submesh.IndexCount <= meshComponent.Indices.size(), "Submesh indices out of bounds");
					
    				    const uint32_t* indices = meshComponent.Indices.data() + submesh.IndexStart;
					
    				    for (uint32_t index = 0; index < submesh.IndexCount; index += 3)
    				    {
    				        Triangle tri;
    				        tri.mMaterialIndex = 0; // hen doesnt support phys materials ffs

    				        const math::Vec3& v0 = meshComponent.Vertices[indices[index + 0]] * transformComp.LocalScale;
    				        const math::Vec3& v1 = meshComponent.Vertices[indices[index + 1]] * transformComp.LocalScale;
    				        const math::Vec3& v2 = meshComponent.Vertices[indices[index + 2]] * transformComp.LocalScale;
						
    				        tri.mV[0] = Float3(v0.x, v0.y, v0.z);
    				        tri.mV[1] = Float3(v1.x, v1.y, v1.z);
    				        tri.mV[2] = Float3(v2.x, v2.y, v2.z);
						
    				        list.push_back(tri);
    				    }
    				}

					if (list.empty())
					{
					    return;
					}

					MeshShapeSettings settings(list);
					settings.SetEmbedded();
					shapeResult = settings.Create();

					break;
				}
			}

			if (!shapeResult.IsValid())
			{
			    if (shapeResult.HasError())
			    {
			        HEN_ERROR("[hen::physics] Rigidbody creation failed: " + std::string(shapeResult.GetError().c_str()));
			    }
			    else
			    {
			        HEN_ERROR("[hen::physics] Rigidbody creation failed: Unknown error");
			    }
				
			    return;
			}

			PhysicsLevel& physicsLevel = GetPhysicsLevel(level);
			BodyInterface& interface = physicsLevel.System.GetBodyInterface();
			
			Mat44 worldMat = Cast(transformComp.GetWorldMatrix());
			Vec3 offset = Cast(rigidBodyComp.Offset);

			Vec3 position = worldMat.GetTranslation();
			Quat rotation = worldMat.GetQuaternion().Normalized();

			const EMotionType motionType = rigidBodyComp.Mass == 0 ? EMotionType::Static : (rigidBodyComp.Kinematic ? EMotionType::Kinematic : EMotionType::Dynamic);
			const ObjectLayer layer = motionType == EMotionType::Static ? LAYERS::STATIC : LAYERS::DYNAMIC;

			BodyCreationSettings settings(shapeResult.Get().GetPtr(), offset + position, rotation, motionType, layer);

			settings.mMassPropertiesOverride.mMass = motionType == EMotionType::Dynamic ? rigidBodyComp.Mass : 1;
			settings.mFriction = rigidBodyComp.Friction;
			settings.mRestitution = rigidBodyComp.Restitution;
			settings.mLinearDamping = rigidBodyComp.LinearDamping;
			settings.mAngularDamping = rigidBodyComp.AngularDamping;
			settings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
			settings.mAllowSleeping = !rigidBodyComp.DisableDeactivation;
			settings.mMotionQuality = MotionQuality;

			const EActivation activation = rigidBodyComp.StartDeactivated ? EActivation::DontActivate : EActivation::Activate;

			BodyID newBody = interface.CreateAndAddBody(settings, activation);

			if (newBody.IsInvalid())
			{
				HEN_ERROR("[hen::physics] Failed to create rigidbody for entity: " + entity.GetComponent<level::NameComponent>().Name);
				return;
			}

			RigidBody& physicsObject = GetRigidBody(rigidBodyComp);
			physicsObject.BodyHandle = newBody;
			physicsObject.Shape = shapeResult.Get();
			physicsObject.ParentPhysicsLevel = level.PhysicsLevel;
			physicsObject.Entity = entity;

			physicsObject.Offset.SetTranslation(offset);
			physicsObject.OffsetInverse = physicsObject.Offset.Inversed();
			physicsObject.PreviousPosition = position;
			physicsObject.PreviousRotation = rotation;

			interface.SetUserData(newBody, (uint64_t)&physicsObject);
		}

	}

	static jolt::JobSystemImpl JoltPhysicsJobSystem;
	static jolt::TempAllocatorMalloc JoltPhysicsAllocator;

    void Initialise()
    {
        hen::Timer timer;

		jolt::RegisterDefaultAllocator();
		jolt::Factory::sInstance = new jolt::Factory();
		jolt::RegisterTypes();

		Initialised = true;

        HEN_LOG("[hen::physics] Initialised with Jolt Physics " + std::to_string(JPH_VERSION_MAJOR) + "." + std::to_string(JPH_VERSION_MINOR) + "." + std::to_string(JPH_VERSION_PATCH) + " in " + std::to_string(static_cast<int>(std::round(timer.ElapsedMilliseconds()))) + " ms");
    }

	void Shutdown()
	{
		jolt::UnregisterTypes();

		delete jolt::Factory::sInstance;
		jolt::Factory::sInstance = nullptr;
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

		physLevel.System.SetGravity(jolt::Cast(currentLevel.Gravity));

		level::View rbView = currentLevel.GetView<level::RigidBodyComponent>();

		jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel, deltaTime](jobsystem::DispatchArgs args)
		{
			level::Entity entity = rbView[args.JobIndex];
			
			if (!entity.HasComponent<level::TransformComponent>())
			{
				HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + " doesn't have a transform component");
				return;
			}

			level::RigidBodyComponent& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
			level::TransformComponent& transformComponent = entity.GetComponent<level::TransformComponent>();

			if (rbComponent.PhysicsObject == nullptr)
			{
				jolt::AddRigidBody(*level::GetActiveLevel(), entity);
				return;
			}

			jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

			if (physObj.BodyHandle.IsInvalid())
			{
				jolt::AddRigidBody(*level::GetActiveLevel(), entity);
				return;
			}

			jolt::BodyInterface& bodyInterface = physLevel.System.GetBodyInterface();

			if (rbComponent.Dirty)
			{
				bodyInterface.RemoveBody(physObj.BodyHandle);
				bodyInterface.DestroyBody(physObj.BodyHandle);

				jolt::AddRigidBody(*level::GetActiveLevel(), entity);

				rbComponent.Dirty = false;
				return;
			}

			bodyInterface.SetFriction(physObj.BodyHandle, rbComponent.Friction);
			bodyInterface.SetRestitution(physObj.BodyHandle, rbComponent.Restitution);
			const jolt::EMotionType prevMotionType = bodyInterface.GetMotionType(physObj.BodyHandle);
			const jolt::EMotionType currentMotionType = rbComponent.Mass == 0 ? jolt::EMotionType::Static : (rbComponent.Kinematic ? jolt::EMotionType::Kinematic : jolt::EMotionType::Dynamic);

			if (prevMotionType != currentMotionType)
			{
				bodyInterface.SetMotionType(physObj.BodyHandle, currentMotionType, jolt::EActivation::Activate);
			}

			bodyInterface.ActivateBody(physObj.BodyHandle);

			const jolt::Vec3 position = jolt::Cast(transformComponent.LocalPosition);
			const jolt::Quat rotation = jolt::Cast(transformComponent.LocalRotation);
			
			jolt::Mat44 matrix = jolt::Mat44::sTranslation(position) * jolt::Mat44::sRotation(rotation);
			matrix = matrix * physObj.Offset;

			if (cvar_SimulationEnabled.GetBool())
			{
				if (currentMotionType == jolt::EMotionType::Kinematic)
				{
					bodyInterface.MoveKinematic(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), physLevel.GetKinematicDT(deltaTime));
				}
				else if (currentMotionType == jolt::EMotionType::Static || !bodyInterface.IsActive(physObj.BodyHandle))
				{
					bodyInterface.SetPositionAndRotation(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), jolt::EActivation::DontActivate);
				}
			}
			else
			{
				physObj.PreviousPosition = position;
				physObj.PreviousRotation = rotation;
				bodyInterface.SetPositionAndRotation(physObj.BodyHandle, matrix.GetTranslation(), matrix.GetQuaternion().Normalized(), JPH::EActivation::Activate);
			}
		});

		jobsystem::Wait();

		if (cvar_SimulationEnabled.GetBool())
    	{
    	    const float fixedStep = 1.0f / cvar_HZ.GetInt();

    	    physLevel.Accumulator += deltaTime;
    	    physLevel.Accumulator = math::Clamp(physLevel.Accumulator, 0.0f, fixedStep * cvar_Accuracy.GetInt());

    	    while (physLevel.Accumulator >= fixedStep)
    	    {
    	        if (cvar_InterpolationEnabled.GetBool())
    	        {
    	            jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel](jobsystem::DispatchArgs args)
    	            {
    	                level::Entity entity = rbView[args.JobIndex];
    	                level::RigidBodyComponent& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
    	                jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

    	                if (physObj.BodyHandle.IsInvalid())
						{
    	                    return;
						}

    	                jolt::BodyInterface& bodyInterface = physLevel.System.GetBodyInterface();
    	                jolt::Mat44 matrix = bodyInterface.GetWorldTransform(physObj.BodyHandle);
    	                matrix = matrix * physObj.OffsetInverse;

    	                physObj.PreviousPosition = matrix.GetTranslation();
    	                physObj.PreviousRotation = matrix.GetQuaternion().Normalized();
    	            });

					jobsystem::Wait();
    	        }

    	        physLevel.System.Update(fixedStep, 1, &JoltPhysicsAllocator, &JoltPhysicsJobSystem);
    	        physLevel.Accumulator -= fixedStep;
    	    }

    	    physLevel.Alpha = math::Clamp(physLevel.Accumulator / fixedStep, 0.0f, 1.0f);
    	}


		jobsystem::Dispatch(rbView.Size(), 64, [&rbView, &physLevel](jobsystem::DispatchArgs args) 
		{
			level::Entity entity = rbView[args.JobIndex];

			if (!entity.HasComponent<level::TransformComponent>())
			{
				HEN_WARN("[hen::physics] Entity: " + entity.GetComponent<level::NameComponent>().Name + " doesn't have a transform component");
				return;
			}

			level::RigidBodyComponent& rbComponent = entity.GetComponent<level::RigidBodyComponent>();
			level::TransformComponent& transformComponent = entity.GetComponent<level::TransformComponent>();

			jolt::RigidBody& physObj = jolt::GetRigidBody(rbComponent);

			if (physObj.BodyHandle.IsInvalid())
			{
				return;
			}

			jolt::BodyInterface& bodyInterface = physLevel.System.GetBodyInterface();

			if (bodyInterface.GetMotionType(physObj.BodyHandle) != jolt::EMotionType::Dynamic)
			{
				return;
			}

			jolt::Mat44 matrix = bodyInterface.GetWorldTransform(physObj.BodyHandle);
			matrix = matrix * physObj.OffsetInverse;

			jolt::Vec3 position = matrix.GetTranslation();
			jolt::Quat rotation = matrix.GetQuaternion().Normalized();

			if (cvar_InterpolationEnabled.GetBool())
			{
				position = position * physLevel.Alpha + physObj.PreviousPosition * (1 - physLevel.Alpha);
				rotation = physObj.PreviousRotation.SLERP(rotation, physLevel.Alpha);
			}

			transformComponent.SetLocalPosition(jolt::Cast(position));
			transformComponent.SetLocalRotation(jolt::Cast(rotation));
		});

		jobsystem::Wait();
    }

	math::Vec3 GetLinearVelocity(level::RigidBodyComponent& rbComponent)
	{
		if (!rbComponent.PhysicsObject)
		{
			return math::Vec3(0.0f);
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			return jolt::Cast(physLevel->System.GetBodyInterfaceNoLock().GetLinearVelocity(jolt::GetRigidBody(rbComponent).BodyHandle));
		}

		return math::Vec3(0.0f);
	}    

    math::Vec3 GetAngularVelocity(level::RigidBodyComponent& rbComponent)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return math::Vec3(0.0f);
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			return jolt::Cast(physLevel->System.GetBodyInterfaceNoLock().GetAngularVelocity(jolt::GetRigidBody(rbComponent).BodyHandle));
		}

		return math::Vec3(0.0f);
	}

	void AddImpulse(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return;
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			physLevel->System.GetBodyInterfaceNoLock().AddImpulse(jolt::GetRigidBody(rbComponent).BodyHandle, jolt::Cast(impulse));
		}
	}

    void AddImpulseAt(level::RigidBodyComponent& rbComponent, const math::Vec3& impulse, const math::Vec3& position)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return;
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			physLevel->System.GetBodyInterfaceNoLock().AddImpulse(jolt::GetRigidBody(rbComponent).BodyHandle, jolt::Cast(impulse), jolt::Cast(position));
		}
	}

    void AddForce(level::RigidBodyComponent& rbComponent, const math::Vec3& force)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return;
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			physLevel->System.GetBodyInterfaceNoLock().AddForce(jolt::GetRigidBody(rbComponent).BodyHandle, jolt::Cast(force));
		}
	}

	void AddForceAt(level::RigidBodyComponent& rbComponent, const math::Vec3& force, const math::Vec3& position)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return;
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			physLevel->System.GetBodyInterfaceNoLock().AddForce(jolt::GetRigidBody(rbComponent).BodyHandle, jolt::Cast(force), jolt::Cast(position));
		}
	}

    void AddTorque(level::RigidBodyComponent& rbComponent, const math::Vec3& torque)
	{
		if (!rbComponent.PhysicsObject)
		{
			HEN_ERROR("[hen::physics] Rigidbody component provided doesn't have a rigidbody");
			return;
		}

		if (jolt::PhysicsLevel* physLevel = reinterpret_cast<jolt::PhysicsLevel*>(jolt::GetRigidBody(rbComponent).ParentPhysicsLevel.get()))
		{
			physLevel->System.GetBodyInterfaceNoLock().AddTorque(jolt::GetRigidBody(rbComponent).BodyHandle, jolt::Cast(torque));
		}
	}
	
	level::RayResult CastRay(const level::Ray& ray)
	{
		level::RayResult result;

		level::Level* currentLevel = level::GetActiveLevel();

		if (!currentLevel)
		{
			return result;
		}

		jolt::PhysicsLevel& physLevel = jolt::GetPhysicsLevel(*currentLevel);

		jolt::ClosestHitCollisionCollector<jolt::CastRayCollector> collector;
		jolt::RayCastSettings settings;
		jolt::RRayCast rayCast (jolt::Cast(ray.Origin), jolt::Cast(ray.Direction) * ray.Maximum);

		physLevel.System.GetNarrowPhaseQuery().CastRay(rayCast, settings, collector);

		if (collector.HadHit())
		{
			const jolt::RayCastResult& hit = collector.mHit;

        	jolt::Vec3 hitPos = rayCast.mOrigin + rayCast.mDirection * hit.mFraction;
			jolt::Vec3 hitNormal;
			jolt::RigidBody* hitObj;

			jolt::BodyLockRead lock(physLevel.System.GetBodyLockInterface(), hit.mBodyID);

			if (lock.Succeeded())
			{
			    const jolt::Body& body = lock.GetBody();

			    hitNormal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, hitPos);
				hitObj = reinterpret_cast<jolt::RigidBody*>(body.GetUserData());
			}

        	result.Hit = true;
        	result.HitPosition = jolt::Cast(hitPos);
			result.HitNormal = jolt::Cast(hitNormal);
			result.HitEntity = &hitObj->Entity;
		}

		return result;
	}

}