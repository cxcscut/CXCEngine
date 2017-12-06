#ifndef CXC_PHYSICS_RIGIDBODY_H
#define CXC_PHYSICS_RIGIDBODY_H

#define GEOMS_MAX_NUM 1

#include "ode/ode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace cxc {

	class RigidBody3D {

	public:

		RigidBody3D();
		virtual ~RigidBody3D();

		// Memory allocation and deallocation
	public:

		dBodyID createRigidBody(dWorldID world);
		void destroyRigidBody(dBodyID body);

		void Initialize() noexcept;

		// Properties 
	public:

		void setPossition(dReal x, dReal y, dReal z) noexcept;
		void setRotation(const glm::mat4 rot) noexcept;
		void setLinearVelocity(dReal x, dReal y, dReal z) noexcept;
		void setAngularVelocity(dReal x, dReal y, dReal z) noexcept;

		glm::vec3 getPosition() const;
		glm::mat4 getRotation() const;
		glm::vec3 getLinearVelocity() const;
		glm::vec3 getAngularVelocity() const;

		void setMass(const dMass *mass) noexcept;
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

		dJointID createJoint(int type,dJointGroupID joint_group,const dContact *contact = nullptr) noexcept;
		void destroyJoint(dJointID joint) noexcept;

		dJointGroupID createJointGroup() noexcept;
		void destroyJointGroup(dJointGroupID joint_group) noexcept;
		void emptyJointGroup(dJointGroupID joint_group) noexcept;

		void attachJoint(dJointID joint, const RigidBody3D &rb1, const RigidBody3D &rb2) noexcept;
		std::shared_ptr<RigidBody3D> getRigidBodyPtr(dJointID joint, int index) const noexcept;

		bool areConnected(const std::shared_ptr<RigidBody3D> &prb1, const std::shared_ptr<RigidBody3D> &prb2) const noexcept;

		// Geometry
	public:

		void createGeom(int classtype) noexcept;
		void destroyGemo(dGeomID geo) noexcept;

		void setGemo(dGeomID geo) noexcept;

		void setGeomPosition(dGeomID geo,dReal x, dReal y,dReal z) noexcept;
		void setGeomRotation(dGeomID geo,dReal x,dReal y,dReal z) noexcept;

		glm::vec3 getGeomPosition() const noexcept;
		glm::mat4 getGeomRotation() const noexcept;

		dSpaceID getGeomSpace() const noexcept;
		int getGeomClass(dGeomID geo) const noexcept;

		void enableGeom(dGeomID geo) noexcept;
		void disableGeom(dGeomID geo) noexcept;
		bool isGeomEnable(dGeomID geo) const noexcept;

	private:

		std::vector<dGeomID> Geoms;
		dWorldID m_WorldID;
		dBodyID m_BodyID;

	};
}

#endif // CXC_PHYSICS_RIGIDBODY_H