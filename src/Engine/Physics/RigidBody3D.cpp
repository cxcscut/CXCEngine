#include "RigidBody3D.h"
#include <iostream>
namespace cxc {
		
	RigidBody3D::RigidBody3D():
		m_WorldID(0),
		m_BodyID(0),Initialized(false),m_GravityMode(0)
	{

		m_pCollider = std::make_shared<Collider3D>();
	}

	RigidBody3D::~RigidBody3D()
	{
		if(m_BodyID)
			destroyRigidBody();

	}

	void RigidBody3D::addCollider(dSpaceID space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept
	{

		m_pCollider->createTriMeshGeom(space, vertices, indices);

		m_pCollider->associateRigidBody(m_BodyID);
	}

	std::shared_ptr<Collider3D> RigidBody3D::getColliderPtr() noexcept
	{
		return m_pCollider;
	}

	void RigidBody3D::createRigidBody(dWorldID world) noexcept
	{
		m_WorldID = world;

		m_BodyID = dBodyCreate(m_WorldID);

		dBodySetGravityMode(m_BodyID,m_GravityMode);

		Initialized = true;

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

	void RigidBody3D::Initialize(dWorldID world) noexcept
	{

		auto _m = m_pCollider->GetMass();

		dMassTranslate(&_m,-_m.c[0],-_m.c[1],-_m.c[2]);

		dBodySetMass(m_BodyID,&_m);

	}

	void RigidBody3D::UpdateMeshTransform() noexcept
	{
		auto geom_id = m_pCollider->getGeomID();

		if (dGeomGetClass(geom_id) == dTriMeshClass) {

			const dReal* pos = dGeomGetPosition(geom_id);
			const dReal* rot = dGeomGetRotation(geom_id);

			const dReal trans_mat[16] = {
				rot[0],rot[4],rot[8],0,
				rot[1],rot[5],rot[9],0,
				rot[2],rot[6],rot[10],0,
				pos[0],pos[1],pos[2],1
			};
	
			dGeomTriMeshSetLastTransform(geom_id, *(dMatrix4*)(&trans_mat));
		}
	}

	glm::mat4 RigidBody3D::getTransMatrix() const noexcept
	{
		auto R = dGeomGetRotation(m_pCollider->getGeomID());
		auto pos = dGeomGetPosition(m_pCollider->getGeomID());

		return glm::transpose(glm::mat4({
			R[0],R[1],R[2],pos[0],
			R[4],R[5],R[6],pos[1],
			R[8],R[9],R[10],pos[2],
			0,0,0,1
		}));
	}

	void RigidBody3D::setRotation(const glm::mat3 rot) noexcept
	{
		dMatrix3 rot_mat = { rot[0][0],rot[1][0],rot[2][0],0,
							rot[0][1],rot[1][1],rot[2][1],0,
							rot[0][2],rot[1][2],rot[2][2],0};

		dBodySetRotation(m_BodyID,rot_mat); 
		dGeomSetRotation(m_pCollider->getGeomID(),rot_mat);
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
		std::vector<dReal> _rot;
		for (auto i = 0; i < 12; i++)
			_rot.push_back(rot[i]);

		ret = glm::mat3({
			rot[0],rot[4],rot[8],
			rot[1],rot[5],rot[9],
			rot[2],rot[6],rot[10]
		});
		
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
							const glm::mat3 &I) noexcept
	{
		dMass mass;

		//dBodySetMass(m_BodyID,&mass);	
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
		if (Initialized)
			dBodySetGravityMode(m_BodyID, mode);
		m_GravityMode = mode;
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