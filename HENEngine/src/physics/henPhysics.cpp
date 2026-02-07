#include "physics/henPhysics.h"

#include "vendor/JoltPhysics/Jolt/Jolt.h"
#include "vendor/JoltPhysics/Jolt/RegisterTypes.h"
#include "vendor/JoltPhysics/Jolt/Core/Factory.h"
#include "vendor/JoltPhysics/Jolt/Core/TempAllocator.h"
#include "vendor/JoltPhysics/Jolt/Core/JobSystemThreadPool.h"
#include "vendor/JoltPhysics/Jolt/Physics/PhysicsSettings.h"
#include "vendor/JoltPhysics/Jolt/Physics/PhysicsSystem.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.h"
#include "vendor/JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.h"
#include "vendor/JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.h"
#include "vendor/JoltPhysics/Jolt/Physics/Body/BodyActivationListener.h"

JPH_SUPPRESS_WARNINGS

#include "core/henTimer.h"
#include "core/henJobSystem.h"
#include "tools/henConsole.h"

#include <thread>

using namespace JPH::literals;

namespace hen::physics
{
    namespace Layers
    {
        static constexpr JPH::ObjectLayer STATIC = 0;
        static constexpr JPH::ObjectLayer DYNAMIC = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    }

    namespace BroadPhaseLayers
    {
    	static constexpr JPH::BroadPhaseLayer STATIC(0);
    	static constexpr JPH::BroadPhaseLayer DYNAMIC(1);
    	static constexpr JPH::uint NUM_LAYERS(2);
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
    	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    	{
    		switch (inObject1)
    		{
    		case Layers::STATIC:
    			return inObject2 == Layers::DYNAMIC; // Non moving only collides with moving
    		case Layers::DYNAMIC:
    			return true; // Moving collides with everything
    		default:
    			JPH_ASSERT(false);
    			return false;
    		}
    	}
    };

    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
    	BPLayerInterfaceImpl()
    	{
    		mObjectToBroadPhase[Layers::STATIC] = BroadPhaseLayers::STATIC;
    		mObjectToBroadPhase[Layers::DYNAMIC] = BroadPhaseLayers::DYNAMIC;
    	}

    	virtual JPH::uint GetNumBroadPhaseLayers() const override
    	{
    		return BroadPhaseLayers::NUM_LAYERS;
    	}

    	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    	{
    		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    		return mObjectToBroadPhase[inLayer];
    	}

    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    	virtual const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    	{
    		switch ((JPH::BroadPhaseLayer::Type)inLayer)
    		{
    		    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::STATIC:	
                    return "STATIC";
    		    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::DYNAMIC:		
                    return "DYNAMIC";
    		    default:													
                    JPH_ASSERT(false); return "INVALID";
    		}
    	}
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

    private:
    	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
    	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    	{
    		switch (inLayer1)
    		{
    		case Layers::STATIC:
    			return inLayer2 == BroadPhaseLayers::STATIC;
    		case Layers::DYNAMIC:
    			return true;
    		default:
    			JPH_ASSERT(false);
    			return false;
    		}
    	}
    };

	class MyContactListener : public JPH::ContactListener
	{
	public:
		virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
		{
			HEN_LOG("Contact validate callback");
			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
		{
			HEN_LOG("A contact was added");
		}

		virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
		{
			HEN_LOG("A contact was persisted");
		}

		virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
		{
			HEN_LOG("A contact was removed");
		}
	};

	class MyBodyActivationListener : public JPH::BodyActivationListener
	{
	public:
		virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
		{
			HEN_LOG("A body got activated");
		}

		virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
		{
			HEN_LOG("A body went to sleep");
		}
	};


	static BPLayerInterfaceImpl BroadPhaseInterface;
	static ObjectVsBroadPhaseLayerFilterImpl ObjectBroadphaseFilter;
	static ObjectLayerPairFilterImpl ObjectObjectLayerFilter;

	static MyContactListener ContactListener;

	static JPH::PhysicsSystem System;
	static std::unique_ptr<JPH::JobSystemThreadPool> JobSystem;
	static std::unique_ptr<JPH::TempAllocatorImpl> Allocator;

	static const JPH::uint MaxBodies = 1024;
	static const JPH::uint NumBodyMutexes = 0;
	static const JPH::uint MaxBodyPairs = 1024;
	static const JPH::uint MaxContactConstraints = 1024;

	static JPH::BodyID SphereID;

    void Initialise()
    {
        hen::Timer timer;

        JPH::RegisterDefaultAllocator();

        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        Allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024); // 10mb

        JobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

		System.Init(MaxBodies, NumBodyMutexes, MaxBodyPairs, MaxContactConstraints, BroadPhaseInterface, ObjectBroadphaseFilter, ObjectObjectLayerFilter);

		System.SetContactListener(&ContactListener);

		JPH::BodyInterface &body_interface = System.GetBodyInterface();

		JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));
		floor_shape_settings.SetEmbedded();

		JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
		JPH::ShapeRefC floor_shape = floor_shape_result.Get();

		JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::STATIC);

		JPH::Body *floor = body_interface.CreateBody(floor_settings);

		body_interface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

		JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0_r, 2.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::DYNAMIC);
		SphereID = body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);

		body_interface.SetLinearVelocity(SphereID, JPH::Vec3(0.0f, -5.0f, 0.0f));

		System.OptimizeBroadPhase();

        HEN_LOG("[hen::physics] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void Run(float hz)
    {	
		System.Update(hz, 1, Allocator.get(), JobSystem.get());

		if (System.GetBodyInterface().IsActive(SphereID))
		{
			JPH::RVec3 position = System.GetBodyInterface().GetCenterOfMassPosition(SphereID);
			JPH::Vec3 velocity = System.GetBodyInterface().GetLinearVelocity(SphereID);

			HEN_LOG("Position = (" + std::to_string(position.GetX()) + ", " + std::to_string(position.GetY()) + ", " + std::to_string(position.GetZ()) + "), Velocity = (" + std::to_string(velocity.GetX()) + ", " + std::to_string(velocity.GetY()) + ", " + std::to_string(velocity.GetZ()) + ")" );
		}
    }

	void Shutdown()
	{

		JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}
}