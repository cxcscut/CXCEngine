#include "RigidBody3D.h"

namespace cxc {
		
	RigidBody3D::RigidBody3D():
		m_WorldID(0),
		m_BodyID(0)
	{

	}

	RigidBody3D::RigidBody3D(dWorldID world_id)
	{
		m_WorldID = world_id;
	}

	RigidBody3D::~RigidBody3D()
	{
		if (m_BodyID)
			destroyRigidBody();

	}

	void RigidBody3D::addCollider(dSpace space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept
	{
		m_pCollider = std::make_shared<Collider3D>();

		m_pCollider->createTriMeshGeom(space, vertices, indices);
	}

	void RigidBody3D::createRigidBody() noexcept
	{
		if (!m_WorldID || m_BodyID)
			return;

		m_BodyID = dBodyCreate(m_WorldID);
	}

	void RigidBody3D::destroyRigidBody() noexcept
	{
		if (m_BodyID)
			dBodyDestroy(m_BodyID);
	}

	void RigidBody3D::setPossition(dReal x, dReal y, dReal z) noexcept
	{
		if (m_BodyID)
			dBodySetPosition(m_BodyID,x,y,z);
	}

	void RigidBody3D::setRotation(const glm::mat4 rot) noexcept
	{
		if (m_BodyID)
		{
			dMatrix3 rot_mat = { rot[0][0],rot[1][0],rot[2][0],rot[3][0],
								rot[0][1],rot[1][1],rot[2][1],rot[3][1],
								rot[0][2],rot[1][2],rot[2][2],rot[3][2] };

			dBodySetRotation(m_BodyID,rot_mat);
		}
	}

	void RigidBody3D::setLinearVelocity(dReal x, dReal y, dReal z) noexcept
	{
		if (m_BodyID)
			dBodySetLinearVel(m_BodyID,x,y,z);
	}

	void RigidBody3D::setAngularVelocity(dReal x, dReal y, dReal z) noexcept
	{
		if (m_BodyID)
			dBodySetAngularVel(m_BodyID,x,y,z);
	}

	glm::vec3 RigidBody3D::getPosition() const
	{
		glm::vec3 ret;

		if (m_BodyID)
		{
			const dReal *pos;
			pos = dBodyGetPosition(m_BodyID);

			ret.x = pos[0]; ret.y = pos[1]; ret.z = pos[2];
		}

		return ret;
	}

	glm::mat4 RigidBody3D::getRotation() const
	{
		glm::mat4 ret;

		if (m_BodyID)
		{
			const dReal *rot = dBodyGetRotation(m_BodyID);
			ret = glm::transpose(glm::mat4({
				rot[0],rot[1],rot[2],rot[3],
				rot[4],rot[5],rot[6],rot[7],
				rot[8],rot[9],rot[10],rot[11],
				0,0,0,1
			}));
		}

		return ret;
	}

	glm::vec3 RigidBody3D::getLinearVelocity() const
	{
		glm::vec3 linear_vel;

		if (m_BodyID)
		{
			const dReal * vel = dBodyGetLinearVel(m_BodyID);

			linear_vel.x = vel[0]; linear_vel.y = vel[1]; linear_vel.z = vel[2];
		}

		return linear_vel;
	}

	glm::vec3 RigidBody3D::getAngularVelocity() const
	{
		glm::vec3 angular_vel;

		if (m_BodyID)
		{
			const dReal * vel = dBodyGetAngularVel(m_BodyID);

			angular_vel.x = vel[0]; angular_vel.y = vel[1]; angular_vel.z = vel[2];
		}

		return angular_vel;
	}

	void RigidBody3D::setMass(dReal MassValue,
							const glm::vec3 & center_pos,
							const glm::mat3 &iner_mat) noexcept
	{
		dMass * mass = nullptr;
		if (m_BodyID)
		{
			dMassSetParameters(mass,MassValue,
							center_pos.x,center_pos.y,center_pos.z,
							iner_mat[0][0],iner_mat[1][1],iner_mat[2][2],
							iner_mat[1][1],iner_mat[2][1],iner_mat[2][2]);

			dBodySetMass(m_BodyID,mass);
		}

	}

	void RigidBody3D::getMass(dMass *mass) const noexcept
	{
		if (m_BodyID)
		{
			dBodyGetMass(m_BodyID,mass);
		}

		mass = nullptr;
	}

	void RigidBody3D::addForce(dReal fx, dReal fy, dReal fz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddForce(m_BodyID,fx,fy,fz);
		}
	}

	void RigidBody3D::addTorque(dReal fx, dReal fy, dReal fz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddTorque(m_BodyID,fx,fy,fz);
		}
	}

	void RigidBody3D::addForceAtPos(dReal fx, dReal fy, dReal fz,
									dReal px, dReal py, dReal pz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddForceAtPos(m_BodyID,
								fx,fy,fz,
								px,py,pz);
		}
	}

	void RigidBody3D::addRelForceAtPos(dReal fx, dReal fy, dReal fz,
										dReal px, dReal py, dReal pz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddRelForceAtPos(m_BodyID,
								fx,fy,fz,
								px,py,pz);
		}
	}

	void RigidBody3D::addRelForce(dReal fx, dReal fy, dReal fz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddRelForce(m_BodyID,fx,fy,fz);
		}
	}

	void RigidBody3D::addRelTorque(dReal fx, dReal fy, dReal fz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddRelForce(m_BodyID, fx, fy, fz);
		}
	}

	void RigidBody3D::addForceAtRelPos(dReal fx, dReal fy, dReal fz,
									dReal px, dReal py, dReal pz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddForceAtRelPos(m_BodyID,
								fx, fy, fz,
								px, py, pz);
		}
	}

	void RigidBody3D::addRelForceAtRelPos(dReal fx, dReal fy, dReal fz,
										dReal px, dReal py, dReal pz) noexcept
	{
		if (m_BodyID)
		{
			dBodyAddRelForceAtRelPos(m_BodyID,
									fx, fy, fz,
									px, py, pz);
		}
	}

	glm::vec3 RigidBody3D::getForce() const noexcept
	{
		glm::vec3 ret;

		if (m_BodyID)
		{
			const dReal *force = dBodyGetForce(m_BodyID);
			ret.x = force[0]; ret.y = force[1]; ret.z = force[2];
		}

		return ret;
	}

	glm::vec3 RigidBody3D::getTorque() const noexcept
	{
		glm::vec3 ret;

		if (m_BodyID)
		{
			const dReal *torque = dBodyGetForce(m_BodyID);
			ret.x = torque[0]; ret.y = torque[1]; ret.z = torque[2];
		}

		return ret;
	}

	void RigidBody3D::setForce(dReal x, dReal y, dReal z) noexcept
	{
		if (m_BodyID)
		{
			dBodySetForce(m_BodyID,x,y,z);
		}
	}

	void RigidBody3D::setTorque(dReal x, dReal y, dReal z) noexcept
	{
		if (m_BodyID)
		{
			dBodySetTorque(m_BodyID, x, y, z);
		}
	}

	void RigidBody3D::setGravityMode(int mode) noexcept
	{
		if (m_BodyID)
		{
			dBodySetGravityMode(m_BodyID, mode);
		}	
	}

	int RigidBody3D::getGravityMode() const
	{
		if (m_BodyID)
		{
			return dBodyGetGravityMode(m_BodyID);
		}

		return -1;
	}

	int RigidBody3D::getJointNum() const
	{
		if (m_BodyID)
			return dBodyGetNumJoints(m_BodyID);

		else return -1;
	}

	dJointID RigidBody3D::getJoint(int index) const
	{
		if (m_BodyID)
		{
			if (index >= dBodyGetNumJoints(m_BodyID))
				return 0;
			else
				return dBodyGetJoint(m_BodyID,index);
		}

		return 0;
	}

	int RigidBody3D::getJointType(dJointID joint) const
	{
		return dJointGetType(joint);
	}

	dJointID RigidBody3D::createJoint(int type, dJointGroupID joint_group,const dContact * contact) noexcept
	{
		switch (type)
		{
		case dJointTypeBall:
			return dJointCreateBall(m_WorldID,joint_group);
			break;
		case dJointTypeHinge:
			return dJointCreateHinge(m_WorldID,joint_group);
			break;
		case dJointTypeSlider:
			return dJointCreateSlider(m_WorldID,joint_group);
			break;
		case dJointTypeContact:
			return dJointCreateContact(m_WorldID,joint_group,contact);
			break;
		case dJointTypeUniversal:
			return dJointCreateUniversal(m_WorldID,joint_group);
			break;
		case dJointTypeHinge2:
			return dJointCreateHinge2(m_WorldID,joint_group);
			break;
		case dJointTypeFixed:
			return dJointCreateFixed(m_WorldID,joint_group);
			break;
		case dJointTypeAMotor:
			return dJointCreateAMotor(m_WorldID,joint_group);
			break;
		default:
			return 0;
			break;
		}
	}

	void RigidBody3D::destroyJoint(dJointID joint) noexcept
	{
		if (joint)
			dJointDestroy(joint);
	}
}