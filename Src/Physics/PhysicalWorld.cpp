#include "Physics/PhysicalWorld.h"
#include "Physics/Collider3D.h"
#include "Components/CColliderComponent.h"
#include "World/World.h"
#include "Actor/CPawn.h"

namespace cxc
{
	// Collision detection callback function
	void PhysicalWorld::nearCallback(void *data, dGeomID o1, dGeomID o2) noexcept
	{
		int num;

		dContact contacts[MAX_CONTACT_NUM];

		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;

		Collider3D* Collider1 = nullptr, *Collider2 = nullptr;

		if (o1)
			Collider1 = reinterpret_cast<Collider3D*>(dGeomGetData(o1));
		if (o2)
			Collider2 = reinterpret_cast<Collider3D*>(dGeomGetData(o2));

		if ((Collider1 && Collider1->GetCollisionChannel() == eCollisionChannel::CollisionFree) ||
			(Collider2 && Collider1->GetCollisionChannel() == eCollisionChannel::CollisionFree))
			return;

		// Do not check collision if channels are not match
		if (Collider1->GetCollisionChannel() != Collider2->GetCollisionChannel())
			return;

		for (num = 0; num < MAX_CONTACT_NUM; ++num)
		{
			contacts[num].surface.mode = dContactBounce | dContactSoftCFM;
			contacts[num].surface.mu = dInfinity;
			contacts[num].surface.mu2 = 0;
			contacts[num].surface.bounce = 0.1;
			contacts[num].surface.bounce_vel = 0.1;
			contacts[num].surface.soft_cfm = 0.01;
		}

		int num_contact = dCollide(o1, o2, MAX_CONTACT_NUM, &contacts[0].geom, sizeof(dContact));

		if (num_contact > 0)
		{
			for (int i = 0; i < num_contact; ++i)
			{
				auto pPhysicalWorld = reinterpret_cast<PhysicalWorld*>(data);

				// Invoke the collision callback to notify the collider
				Collider1->CollisionCallback(Collider2->shared_from_this());
				Collider2->CollisionCallback(Collider1->shared_from_this());

				dJointID joint = dJointCreateContact(pPhysicalWorld->PhysicalWorldID, pPhysicalWorld->ContactJoints, contacts + i);
				dJointAttach(joint, dGeomGetBody(o1), dGeomGetBody(o2));
			}
		}
	}

	PhysicalWorld::PhysicalWorld() :
		bODEInitialized(false), PhysicalWorldID(nullptr), ContactJoints(nullptr),
		bPhysicsPaused(false), TopLevelSpace(nullptr),
		Gravity(glm::vec3({ 0,-9.81f,0 }))
	{

	}

	PhysicalWorld::~PhysicalWorld()
	{
		DestroyPhysicalWorld();
	}

	void PhysicalWorld::InitializePhysicalWorld()
	{
		dInitODE();
		bODEInitialized = true;

		// Create physical world
		PhysicalWorldID = dWorldCreate();

		// Setting parameters of world
		dWorldSetERP(PhysicalWorldID, 0.2);
		dWorldSetCFM(PhysicalWorldID, 1e-5);

		// Center and extent defines the size of root blocks
		// Center of the root blocks
		dReal center[3] = { 0,0,0 };

		// Extents of the root blocks
		dReal Extent[3] = { 1000,1000,1000 };

		// Create top-level space using quadtree implementation
		TopLevelSpace = dQuadTreeSpaceCreate(0, center, Extent, 4);

		// Set gravity
		dWorldSetGravity(PhysicalWorldID, Gravity.x, Gravity.y, Gravity.z);

		// Create joint gruop for contact joint
		ContactJoints = dJointGroupCreate(0);
	}

	void PhysicalWorld::DestroyPhysicalWorld()
	{
		// Destroy joint group
		if (ContactJoints)
			dJointGroupDestroy(ContactJoints);

		// Destroy space
		if (TopLevelSpace)
			dSpaceDestroy(TopLevelSpace);

		if (bODEInitialized)
		{
			// Close the ODE context
			dCloseODE();
		}
	}

	void PhysicalWorld::PhysicsTick(float DeltaSeconds)
	{
		if (!bPhysicsPaused)
		{
			dSpaceCollide(TopLevelSpace, reinterpret_cast<void*>(this), &PhysicalWorld::nearCallback);

			dWorldStep(PhysicalWorldID, WOLRD_QUICK_STEPSIZE);

			dJointGroupEmpty(ContactJoints);
		}
	}

	void PhysicalWorld::ActiveGravity()
	{
		if (PhysicalWorldID)
		{
			dWorldSetGravity(PhysicalWorldID, Gravity.x, Gravity.y, Gravity.z);
		}
	}
}