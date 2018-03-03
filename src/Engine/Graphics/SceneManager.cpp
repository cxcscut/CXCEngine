#include "SceneManager.h"

#ifdef WIN32

#include "..\EngineFacade\EngineFacade.h"

#else

#include "../EngineFacade/EngineFacade.h"

#endif // WIN32

#include <iostream>

namespace cxc {

	// Collision detection callback function
	void SceneManager::nearCallback(void *data, dGeomID o1, dGeomID o2) noexcept
	{

		int num;

		dContact contacts[MAX_CONTACT_NUM];

		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;

		auto rgbd3d_ptr1 = reinterpret_cast<Shape*>(dBodyGetData(b1));
		auto rgbd3d_ptr2 = reinterpret_cast<Shape*>(dBodyGetData(b2));

        if(rgbd3d_ptr1->CompareTag() == "collision_free"
            || rgbd3d_ptr2->CompareTag() == "collision_free")
            return ;

		// Do not check collision if tags are the same
		if (rgbd3d_ptr1->CompareTag() == rgbd3d_ptr2->CompareTag())
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
				auto pSceneMgr = reinterpret_cast<SceneManager*>(data);

				// Suspend physical loop
				//EngineFacade::GetInstance()->SuspendPhysics();

				assert(rgbd3d_ptr1);
				assert(rgbd3d_ptr2);

                pSceneMgr->Collision = true;

				//std::cout << rgbd3d_ptr1->GetModelName() << " AND " << rgbd3d_ptr2->GetModelName() << " COLLIDES" << std::endl;

				dJointID joint = dJointCreateContact(pSceneMgr->m_WorldID, pSceneMgr->m_ContactJoints, contacts + i);
				dJointAttach(joint, dGeomGetBody(o1), dGeomGetBody(o2));
			}
		}
	}

	SceneManager::SceneManager()
		: m_ObjectMap(),TotalIndicesNum(0U), m_LightPos(glm::vec3(0, 1500, 1500)),
		m_TopLevelSpace(0),m_WorldID(0), m_ContactJoints(0),Collision(false),
		m_Boundary()
	{
		m_pTextureMgr = TextureManager::GetInstance();
		m_pCamera = std::make_shared<Camera>();
		m_pRendererMgr = RendererManager::GetInstance();
	}

	SceneManager::~SceneManager()
	{
		m_ObjectMap.clear();

		// Destroy joint group
		if (m_ContactJoints)
			dJointGroupDestroy(m_ContactJoints);

		// Destroy space
		if (m_TopLevelSpace)
			dSpaceDestroy(m_TopLevelSpace);
	}

	void SceneManager::releaseBuffers() noexcept
	{
		for (auto pObject : m_ObjectMap)
			pObject.second->releaseBuffers();
	}

	void SceneManager::initResources() noexcept
	{
		// Init buffer objects
		for (auto pObject : m_ObjectMap) {
			pObject.second->InitBuffers();
		}
	}

	void SceneManager::BuildOctree() noexcept
	{
		if (pRoot) 
			return;

		pRoot = std::make_unique<OctreeNode>(m_Boundary,1);

		for (auto piter = m_ObjectMap.begin(); piter != m_ObjectMap.end(); piter++)
			pRoot->InsertObject(piter->second);
		
	}

	const glm::vec3 &SceneManager::GetLightPos() const noexcept
	{
		return m_LightPos;
	}

	void SceneManager::SetLightPos(const glm::vec3 &pos) noexcept
	{
		m_LightPos = pos;
	}

	void SceneManager::BindLightingUniforms(GLuint ProgramID) const
	{

		GLuint LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
		glUniform3f(LightID, m_LightPos.x, m_LightPos.y, m_LightPos.z);

	}

	void SceneManager::UpdateMeshTransMatrix() noexcept
	{
		for (auto object : m_ObjectMap)
			object.second->UpdateMeshTransMatrix();
	}

	void SceneManager::CreatePhysicalWorld(const glm::vec3 &gravity) noexcept
	{
		// Create world
		m_WorldID = dWorldCreate();

		// Setting parameters of world
		dWorldSetERP(m_WorldID, 0.2);
		dWorldSetCFM(m_WorldID, 1e-5);

		// Center and extent defines the size of root blocks
		// Center of the root blocks
		dReal center[3] = {0,0,0};

		// Extents of the root blocks
		dReal Extent[3] = {1000,1000,1000};

		// Create top-level space using quadtree implementation
		m_TopLevelSpace = dQuadTreeSpaceCreate(0, center, Extent, 4);

		// Set gravity
		dWorldSetGravity(m_WorldID,gravity.x,gravity.y,gravity.z);

		// Create joint gruop for contact joint
		m_ContactJoints = dJointGroupCreate(0);
	}

	void SceneManager::InitCameraStatus(GLFWwindow * window) noexcept
	{
		m_pCamera->InitLastTime();

		if (m_pCamera->m_CameraMode == CAMERA_FIXED)
		{
			//Set Keyboard and mouse callback function
			glfwSetKeyCallback(window, KeyBoradCallBack);
			glfwSetMouseButtonCallback(window, MouseCallBack);
			glfwSetScrollCallback(window, ScrollBarCallBack);
		}

		// Set Camera pos
		SetCameraParams(m_pCamera->eye_pos, m_pCamera->origin, m_pCamera->up_vector,
			glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f)
		);

	}

	void SceneManager::SetCameraParams(const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
		const glm::mat4 &ProjectionMatrix) noexcept
	{

		m_pCamera->eye_pos = eye;
		m_pCamera->origin = origin;
		m_pCamera->up_vector = up;
		m_pCamera->SetAllMatrix(glm::lookAt(eye, origin, up), ProjectionMatrix);
		m_pCamera->ComputeAngles();
	}

	void SceneManager::UpdateBoundary(const CXCRect3 &AABB) noexcept
	{
		m_Boundary.max.x = std::fmax(m_Boundary.max.x, AABB.max.x);
		m_Boundary.max.y = std::fmax(m_Boundary.max.y, AABB.max.y);
		m_Boundary.max.z = std::fmax(m_Boundary.max.z, AABB.max.z);

		m_Boundary.min.x = std::fmin(m_Boundary.min.x, AABB.min.x);
		m_Boundary.min.y = std::fmin(m_Boundary.min.y, AABB.min.y);
		m_Boundary.min.z = std::fmin(m_Boundary.min.z, AABB.min.z);
	}

	GLboolean SceneManager::CreateObject(const std::string &Object_name,const std::string &Object_file) noexcept
	{
		auto tmp_object = std::make_shared<Object3D >(Object_name);

		auto ret = tmp_object->LoadOBJFromFile(Object_file);
		if (!ret) return GL_FALSE;

		m_ObjectMap.insert(std::make_pair(Object_name, tmp_object));

		return GL_TRUE;
	}

	void SceneManager::AddObject(const std::string &ObjectName, const std::shared_ptr<Object3D > &ObjectPtr,bool isKinematics) noexcept
	{
		m_ObjectMap.insert(std::make_pair(ObjectName, ObjectPtr));

		ObjectPtr->isKinematics = isKinematics;

		ObjectPtr->InitializeRigidBodies(m_WorldID, m_TopLevelSpace);

		UpdateBoundary(ObjectPtr->GetAABB());
	}

	void SceneManager::DrawScene() noexcept
	{
		if (!pRoot) {
			// if Octree has not been built, draw all the objects
			for (auto pObject : m_ObjectMap)
				if (pObject.second->isEnable())
					pObject.second->DrawObject();
		}
		else
		{
			// Hashmap for objects 
			std::unordered_set<std::shared_ptr<Object3D>> hash;
			std::queue<std::shared_ptr<OctreeNode>> q;

			q.push(pRoot);
			// Tranversing octree tree and perform frustum culling
			while (!q.empty())
			{
				auto pNode = q.front();
				q.pop();

				if (pNode->p_ChildNodes.empty())
				{
					for (auto pObject : pNode->Objects)
					{
						auto AABB = pObject->GetAABB();
						if(m_pCamera->isRectInFrustum(AABB.max, AABB.min))
							hash.insert(pObject);
					}
				}
				else
				{
					for (auto subspace : pNode->p_ChildNodes) {
						if(m_pCamera->isRectInFrustum(subspace->AABB.max,subspace->AABB.min))
							q.push(subspace);
					}
				}
			}

			// Draw the remaining objects
			for (auto piter = hash.begin(); piter != hash.end(); piter++)
				(*piter)->DrawObject();
		}
	}

	void SceneManager::SetCameraMode(CameraModeType mode) noexcept
	{
		m_pCamera->m_CameraMode = mode;
	}

	void SceneManager::UpdateCameraPos(GLFWwindow *window,float x,float y,GLuint height,GLuint width) noexcept
	{
		m_pCamera->ComputeMatrices_Moving(window, x,y,height,width);
	}

	void SceneManager::BindCameraUniforms() const noexcept
	{
		auto SpriteProgramID = m_pRendererMgr->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		m_pCamera->BindCameraUniforms(SpriteProgramID);
	}

	void SceneManager::DeleteObject(const std::string &sprite_name) noexcept
	{
		auto it = m_ObjectMap.find(sprite_name);
		if (it != m_ObjectMap.end())
			m_ObjectMap.erase(it);
	}

	std::shared_ptr<Object3D > SceneManager::GetObject3D(const std::string &sprite_name) const noexcept
	{
		auto it = m_ObjectMap.find(sprite_name);

		if (it != m_ObjectMap.end())
			return it->second;
		else
			return std::shared_ptr<Object3D >(nullptr);
	}

	const std::unordered_map<std::string, std::shared_ptr<Object3D >> &SceneManager::GetObjectMap() const noexcept
	{
		return m_ObjectMap;
	}

	OctreeNode::OctreeNode(const CXCRect3 &SceneSize,uint16_t _depth) : 
		Objects(),p_ChildNodes()
	{
		AABB = SceneSize;
		depth = _depth;
	}

	OctreeNode::OctreeNode() : AABB()
	{

	}

	OctreeNode::~OctreeNode()
	{
	}

	bool OctreeNode::InsertObject(std::shared_ptr<Object3D> pObject) noexcept
	{
		if (!pObject->GetAABB().isIntersected(AABB))
			return false;

		if (p_ChildNodes.empty())
		{
			// haven't partition the node
			Objects.push_back(pObject);

			// Partition space if number of objects excess PARTITION_MIN_NODENUM and depth does not excess 
			// MAX_PARTITION_DEPTH
			if (Objects.size() > PARTITION_MIN_NODENUM && depth < MAX_PARTITION_DEPTH)
			{
				// Partition the space
				SpacePartition();

				// move all the objects to subspace
				while (!Objects.empty()) {
					auto qe = Objects.back();
					Objects.pop_back();

					for (auto subspace : p_ChildNodes)
						if (qe->GetAABB().isIntersected(subspace->AABB))
							subspace->InsertObject(qe);
				}
			}
		}
		else
		{
			// have already partitioned node, find the correct subspace and add it
			for (auto subspace : p_ChildNodes)
				if (pObject->GetAABB().isIntersected(subspace->AABB))
					subspace->InsertObject(pObject);
		}

		return true;
	}

	void OctreeNode::SpacePartition() noexcept
	{
		// Partition the space into 8 subspace
		CXCRect3 subspace0,subspace1,subspace2,subspace3,
			subspace4,subspace5,subspace6,subspace7;

		// Upper space
		// subspace 0
		subspace0.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace0.max.x = AABB.max.x;
		subspace0.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace0.max.y = AABB.max.y;
		subspace0.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace0.max.z = AABB.max.z;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace0,depth + 1));

		// subspace 1
		subspace1.min.x = AABB.min.x;
		subspace1.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace1.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace1.max.y = AABB.max.y;
		subspace1.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace1.max.z = AABB.max.z;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace1, depth + 1));

		// subspace 2
		subspace2.min.x = AABB.min.x;
		subspace2.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace2.min.y = AABB.min.y;
		subspace2.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace2.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace2.max.z = AABB.max.z;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace2, depth + 1));

		// subspace 3
		subspace3.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace3.max.x = AABB.max.x;
		subspace3.min.y = AABB.min.y;
		subspace3.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace3.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace3.max.z = AABB.max.z;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace3, depth + 1));

		// Downspace
		// subspace 4
		subspace4.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace4.max.x = AABB.max.x;
		subspace4.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace4.max.y = AABB.max.y;
		subspace4.min.z = AABB.min.z;
		subspace4.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace4, depth + 1));

		// subspace 5
		subspace5.min.x = AABB.min.x;
		subspace5.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace5.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace5.max.y = AABB.max.y;
		subspace5.min.z = AABB.min.z;
		subspace5.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace5, depth + 1));

		// subspace 6
		subspace6.min.x = AABB.min.x;
		subspace6.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace6.min.y = AABB.min.y;
		subspace6.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace6.min.z = AABB.min.z;
		subspace6.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace6, depth + 1));

		// subspace 7
		subspace3.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace3.max.x = AABB.max.x;
		subspace3.min.y = AABB.min.y;
		subspace3.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace3.min.z = AABB.min.z;
		subspace3.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		p_ChildNodes.push_back(std::make_shared<OctreeNode>(subspace7, depth + 1));
	}

	void OctreeNode::RemoveObject(std::shared_ptr<Object3D> pTarget) noexcept
	{
		std::vector<OctreeNode*> NodesVec;
		if (!FindNode(pTarget, NodesVec))
			return;
		else
		{
			for (auto node : NodesVec)
			{
				Objects.remove_if(
					[&](const std::shared_ptr<Object3D> &p) {
					return p == pTarget;
				});
			}
		}
		
	}

	bool OctreeNode::FindNode(std::shared_ptr<Object3D> pTarget, std::vector<OctreeNode*> &nodes) noexcept
	{
		if (AABB.isIntersected(pTarget->GetAABB()))
		{
			if (p_ChildNodes.empty())
			{
				for(auto pObject : Objects)
					if (pTarget == pObject)
					{
						nodes.push_back(this);
						return true;
					}

				return false;
			}
			else
			{
				bool ret = false;
				for (auto pChild : p_ChildNodes)
					ret = ret || pChild->FindNode(pTarget, nodes);

				return ret;
			}
		}
		else
			return false;
	}

	void OctreeNode::FindObjects(const CXCRect3 &_AABB, std::vector<std::shared_ptr<Object3D>> &objects) noexcept
	{
		if (!AABB.isIntersected(_AABB))
			return;

		if (p_ChildNodes.empty())
		{
			for (auto pObject : Objects)
				if (pObject->GetAABB().isIntersected(_AABB))
					objects.push_back(pObject);
		}
		else
		{
			for (auto node : p_ChildNodes)
				node->FindObjects(_AABB,objects);
		}
	}
}
