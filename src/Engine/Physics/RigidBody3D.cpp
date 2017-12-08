#include "RigidBody3D.h"

namespace cxc {
		
	RigidBody3D::RigidBody3D():
		m_WorldID(0),
		m_BodyID(0)
	{

	}

	RigidBody3D::~RigidBody3D()
	{

		destroyRigidBody();

	}

	void RigidBody3D::addCollider(dSpaceID space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept
	{
		m_pCollider = std::make_unique<Collider3D>();

		m_pCollider->createTriMeshGeom(space, vertices, indices);

		m_pCollider->associateRigidBody(m_BodyID);
	}

	void RigidBody3D::createRigidBody(dWorldID world) noexcept
	{
		m_WorldID = world;

		m_BodyID = dBodyCreate(m_WorldID);
	}

	void RigidBody3D::destroyRigidBody() noexcept
	{
		
		dBodyDestroy(m_BodyID);
	}

	void RigidBody3D::setPossition(dReal x, dReal y, dReal z) noexcept
	{
	
		dBodySetPosition(m_BodyID,x,y,z);
		m_pCollider->setGeomPosition(x, y, z);
	}

	void RigidBody3D::Initialize(dWorldID world,const glm::vec3 &position) noexcept
	{
		createRigidBody(world);

		setPossition(position.x,position.y,position.z);

		dMass *mass;

		getMass(mass);

		setMass(mass->mass, {position.x,position.y,position.z},mass->I);

	}

	void RigidBody3D::setRotation(const glm::mat4 rot) noexcept
	{
		dMatrix3 rot_mat = { rot[0][0],rot[1][0],rot[2][0],
							rot[0][1],rot[1][1],rot[2][1],
							rot[0][2],rot[1][2],rot[2][2]};

		dBodySetRotation(m_BodyID,rot_mat);
		m_pCollider->setGeomRotation(rot);
	}

	void RigidBody3D::setLinearVelocity(dReal x, dReal y, dReal z) noexcept
	{
	
		dBodySetLinearVel(m_BodyID,x,y,z);
	}

	void RigidBody3D::setAngularVelocity(dReal x, dReal y, dReal z) noexcept
	{
	
		dBodySetAngularVel(m_BodyID,x,y,z);
	}

	glm::vec3 RigidBody3D::getPosition() const
	{
		glm::vec3 ret;

		const dReal *pos;
		pos = dBodyGetPosition(m_BodyID);

		ret.x = pos[0]; ret.y = pos[1]; ret.z = pos[2];

		return ret;
	}

	glm::mat3 RigidBody3D::getRotation() const
	{
		glm::mat3 ret;

		const dReal *rot = dBodyGetRotation(m_BodyID);
		ret = glm::transpose(glm::mat3({
			rot[0],rot[1],rot[2],
			rot[3],rot[4],rot[5],
			rot[6],rot[7],rot[8]
		}));
		
		return ret;
	}

	glm::vec3 RigidBody3D::getLinearVelocity() const
	{
		glm::vec3 linear_vel;
		
		const dReal * vel = dBodyGetLinearVel(m_BodyID);

		linear_vel.x = vel[0]; linear_vel.y = vel[1]; linear_vel.z = vel[2];
		return linear_vel;
	}

	glm::vec3 RigidBody3D::getAngularVelocity() const
	{
		glm::vec3 angular_vel;

		const dReal * vel = dBodyGetAngularVel(m_BodyID);
		angular_vel.x = vel[0]; angular_vel.y = vel[1]; angular_vel.z = vel[2];

		return angular_vel;
	}

	void RigidBody3D::setMass(dReal MassValue,
							const glm::vec3 & center_pos,
							dMatrix3 iner_mat) noexcept
	{
		dMass * mass = nullptr;
		
		dMassSetParameters(mass,MassValue,
						center_pos.x,center_pos.y,center_pos.z,
						iner_mat[0],iner_mat[4],iner_mat[8],
						iner_mat[1],iner_mat[2],iner_mat[5]);

		dBodySetMass(m_BodyID,mass);	
	}

	void RigidBody3D::getMass(dMass *mass) const noexcept
	{
		
		dBodyGetMass(m_BodyID,mass);

	}

	void RigidBody3D::addForce(dReal fx, dReal fy, dReal fz) noexcept
	{
		
		dBodyAddForce(m_BodyID,fx,fy,fz);
	}

	void RigidBody3D::addTorque(dReal fx, dReal fy, dReal fz) noexcept
	{
		
		dBodyAddTorque(m_BodyID,fx,fy,fz);
	}

	void RigidBody3D::addForceAtPos(dReal fx, dReal fy, dReal fz,
									dReal px, dReal py, dReal pz) noexcept
	{
		dBodyAddForceAtPos(m_BodyID,
							fx,fy,fz,
							px,py,pz);
	}

	void RigidBody3D::addRelForceAtPos(dReal fx, dReal fy, dReal fz,
										dReal px, dReal py, dReal pz) noexcept
	{
		dBodyAddRelForceAtPos(m_BodyID,
							fx,fy,fz,
							px,py,pz);
	}

	void RigidBody3D::addRelForce(dReal fx, dReal fy, dReal fz) noexcept
	{
		dBodyAddRelForce(m_BodyID,fx,fy,fz);
	}

	void RigidBody3D::addRelTorque(dReal fx, dReal fy, dReal fz) noexcept
	{
		dBodyAddRelForce(m_BodyID, fx, fy, fz);
	}

	void RigidBody3D::addForceAtRelPos(dReal fx, dReal fy, dReal fz,
									dReal px, dReal py, dReal pz) noexcept
	{
		dBodyAddForceAtRelPos(m_BodyID,
							fx, fy, fz,
							px, py, pz);
	}

	void RigidBody3D::addRelForceAtRelPos(dReal fx, dReal fy, dReal fz,
										dReal px, dReal py, dReal pz) noexcept
	{
		
		dBodyAddRelForceAtRelPos(m_BodyID,
								fx, fy, fz,
								px, py, pz);
		
	}

	glm::vec3 RigidBody3D::getForce() const noexcept
	{
		glm::vec3 ret;

		const dReal *force = dBodyGetForce(m_BodyID);
		ret.x = force[0]; ret.y = force[1]; ret.z = force[2];
	
		return ret;
	}

	glm::vec3 RigidBody3D::getTorque() const noexcept
	{
		glm::vec3 ret;

		const dReal *torque = dBodyGetForce(m_BodyID);
		ret.x = torque[0]; ret.y = torque[1]; ret.z = torque[2];
		
		return ret;
	}

	void RigidBody3D::setForce(dReal x, dReal y, dReal z) noexcept
	{
		dBodySetForce(m_BodyID, x, y, z);
	}

	void RigidBody3D::setTorque(dReal x, dReal y, dReal z) noexcept
	{
		dBodySetTorque(m_BodyID, x, y, z);
	}

	void RigidBody3D::setGravityMode(int mode) noexcept
	{
		dBodySetGravityMode(m_BodyID, mode);	
	}

	int RigidBody3D::getGravityMode() const
	{
		
		return dBodyGetGravityMode(m_BodyID);
	}

	int RigidBody3D::getJointNum() const
	{
		return dBodyGetNumJoints(m_BodyID);
	}

	dJointID RigidBody3D::getJoint(int index) const
	{
		
		if (index >= dBodyGetNumJoints(m_BodyID))
			return 0;
		else
			return dBodyGetJoint(m_BodyID,index);
	}

	int RigidBody3D::getJointType(dJointID joint) const
	{
		return dJointGetType(joint);
	}

	dJointID RigidBody3D::createJoint(int type, dJointGroupID joint_group) noexcept
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