#ifndef CXC_PHYSICS_RIGIDBODY_H
#define CXC_PHYSICS_RIGIDBODY_H

#include "ode/ode.h"
#include "Collider3D.h"

#ifdef WIN32

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#else

#include <glm/gtc/matrix_transform.hpp>
#include "ode/ode.h"

#endif // WIN32

#include <memory>

namespace cxc {

	class RigidBody3D {

	public:

		RigidBody3D();
		virtual ~RigidBody3D();

	public:

		void addCollider(dSpaceID space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept;

		glm::mat4 getTransMatrix() const noexcept;

		std::shared_ptr<Collider3D> getColliderPtr() noexcept;

		void UpdateMeshTransform() noexcept;

	public:

		void createRigidBody(dWorldID world,void * user_data) noexcept;
		void destroyRigidBody() noexcept;

		// Properties
	public:

		void SetScalingFactor(const glm::vec3& ScalingVector) noexcept;
		void setPossition(dReal x, dReal y, dReal z) noexcept;
		void setRotation(const glm::mat3 rot) noexcept;
		void setLinearVelocity(dReal x, dReal y, dReal z) noexcept;
		void setAngularVelocity(dReal x, dReal y, dReal z) noexcept;

		bool IsInitialize() const { return Initialized; }
		glm::vec3 GetScalingFactor() const;
		glm::vec3 getPosition() const;
		glm::mat3 getRotation() const;
		glm::vec3 getLinearVelocity() const;
		glm::vec3 getAngularVelocity() const;
		dBodyID GetBodyID() const { return m_BodyID; };

		void setMass(dReal Mass,
					const glm::vec3 & center_pos,
					const glm::mat3 &I) noexcept;

		void getMass(dMass *mass) const noexcept;

		// Dynamics
	public:

		// Dynamics on global frame of reference
		void addForce(dReal fx, dReal fy, dReal fz) noexcept;
		void addTorque(dReal fx, dReal fy, dReal fz) noexcept;
		void addForceAtPos(dReal fx, dReal fy, dReal fz,
			dReal px, dReal py, dReal pz) noexcept;
		void addRelForceAtPos(dReal fx, dReal fy, dReal fz,
			dReal px, dReal py, dReal pz) noexcept;

		// Dynamics on RigidBody-relative frame of reference
		void addRelForce(dReal fx, dReal fy, dReal fz) noexcept;
		void addRelTorque(dReal fx, dReal fy, dReal fz) noexcept;
		void addForceAtRelPos(dReal fx, dReal fy, dReal fz,
			dReal px, dReal py, dReal pz) noexcept;
		void addRelForceAtRelPos(dReal fx, dReal fy, dReal fz,
			dReal px, dReal py, dReal pz) noexcept;

		glm::vec3 getForce() const noexcept;
		glm::vec3 getTorque() const noexcept;

		void setForce(dReal x, dReal y, dReal z) noexcept;
		void setTorque(dReal x, dReal y, dReal z) noexcept;

		// 1 - Gravity on
		// 0 - Gravity off
		void setGravityMode(int mode) noexcept;
		int getGravityMode() const;

		// Joints and joint group
	public:

		int getJointNum() const;
		dJointID getJoint(int index) const;
		int getJointType(dJointID joint) const;

		dJointID createJoint(int type,dJointGroupID joint_group) noexcept;
		void destroyJoint(dJointID joint) noexcept;

	private:

		// Scaling factor of the object
		glm::vec3 ScalingFactor;

		bool Initialized;

		dWorldID m_WorldID;
		dBodyID m_BodyID;

		// Pointer to collider
		std::shared_ptr<Collider3D> m_pCollider;

		int m_GravityMode;
	};
}

#endif // CXC_PHYSICS_RIGIDBODY_H
