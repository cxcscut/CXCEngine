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

		Shape *rgbd3d_ptr1 = nullptr, *rgbd3d_ptr2 = nullptr;

		// Only trimesh user data can be cast into Shape*
		if(b1 && dGeomGetClass(o1) == dTriMeshClass)
			rgbd3d_ptr1 = reinterpret_cast<Shape*>(dBodyGetData(b1));
		if(b2 && dGeomGetClass(o2) == dTriMeshClass)
			rgbd3d_ptr2 = reinterpret_cast<Shape*>(dBodyGetData(b2));

		if ((rgbd3d_ptr1 && rgbd3d_ptr1->CompareTag() == "collision_free") || 
			(rgbd3d_ptr2 && rgbd3d_ptr2->CompareTag() == "collision_free"))
			return;
		
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
		m_Boundary(),m_SceneCenter(glm::vec3(0,0,0)),m_SceneSize(5000.0f)
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

		CXCRect3 AABB;
		AABB.max.x = m_SceneCenter.x + m_SceneSize / 2;
		AABB.max.y = m_SceneCenter.y + m_SceneSize / 2;
		AABB.max.z = m_SceneCenter.z + m_SceneSize / 2;

		AABB.min.x = m_SceneCenter.x - m_SceneSize / 2;
		AABB.min.y = m_SceneCenter.y - m_SceneSize / 2;
		AABB.min.z = m_SceneCenter.z - m_SceneSize / 2;

		pRoot = std::make_shared<OctreeRoot>(m_SceneCenter, AABB);

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
			std::queue<std::shared_ptr<OctreeNode>> q;

			q.push(pRoot);
			
			// Tranversing octree tree and perform frustum culling
			while (!q.empty())
			{
				auto pNode = q.front();
				q.pop();

				if (pNode->isLeaf)
				{
					for (auto pObject : pNode->Objects)
					{
						if (!pObject.second->isEnable())
							continue;

						auto AABB = pObject.second->GetAABB();
						if(m_pCamera->isRectInFrustum(AABB.max, AABB.min))
								hash.insert(pObject.second);
					}
				}
				else
				{
					for (std::size_t k = 0; k < 8; k++)
					{
						auto pChildNode = pRoot->FindNode(pNode->code + std::to_string(k));
						if (m_pCamera->isRectInFrustum(pChildNode->AABB.max, pChildNode->AABB.min))
							q.push(pChildNode);
					}
				}
			}

			//std::cout << "Drawing " << hash.size() << " objects" << std::endl;
			// Draw the remaining objects
			for (auto piter = hash.begin(); piter != hash.end(); piter++)
				(*piter)->DrawObject();

			hash.clear();
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
		auto ActiveProgramID = m_pRendererMgr->GetActiveShader();
		m_pCamera->BindCameraUniforms(ActiveProgramID);
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

	OctreeNode::OctreeNode(const CXCRect3 &SceneSize) : 
		Objects(),AABB(SceneSize),isLeaf(true)
	{

	}

	OctreeNode::~OctreeNode()
	{

	}

	bool OctreeNode::InsertObject(std::shared_ptr<Object3D> pObject) noexcept
	{
		if (!pObject->GetAABB().isIntersected(AABB))
			return false;

		if (isLeaf)
		{
			// haven't partition the node
			Objects.insert(std::make_pair(pObject->GetObjectName(),pObject));
			pObject->m_OctreePtrs.insert(code);

			// Partition space if number of objects excess PARTITION_MIN_NODENUM 
			// and depth does not excess MAX_PARTITION_DEPTH
			if (Objects.size() > PARTITION_MIN_NODENUM && code.size() < MAX_PARTITION_DEPTH)
			{
				// Partition the space
				SpacePartition();
				isLeaf = false;

				// Empty the objects list fo the current node
				for (auto p = Objects.begin(); p != Objects.end();)
				{
					p->second->m_OctreePtrs.erase(p->second->m_OctreePtrs.find(code));
					auto q = p;
					InsertObject(p->second);
					p++;
					Objects.erase(q);
				}
			}
		}
		else
		{
			// have already partitioned node, find the correct subspace and add it
			assert(m_rootPtr);
			for (std::size_t k = 0; k < 8; k++)
			{
				auto pNode = m_rootPtr->FindNode(code + std::to_string(k));
				assert(pNode);

				pNode->InsertObject(pObject);
			}
		}

		return true;
	}

	void OctreeNode::SpacePartition() noexcept
	{
		// Partition the space into 8 subspace
		CXCRect3 subspace0,subspace1,subspace2,subspace3,
			subspace4,subspace5,subspace6,subspace7;

		/*
		* Upper Level :
		* | 0 | 1 |
		* | 2 | 3 |
		* 
		* Bottom Level :
		* | 4 | 5 |
		* | 6 | 7 |
		*/
		auto proot = m_rootPtr;

		assert(proot);
		// Upper space
		// subspace 0
		subspace0.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace0.max.x = AABB.max.x;
		subspace0.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace0.max.y = AABB.max.y;
		subspace0.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace0.max.z = AABB.max.z;
		auto pNode = proot->CreateSpace(code + std::to_string(0),subspace0);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// subspace 1
		subspace1.min.x = AABB.min.x;
		subspace1.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace1.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace1.max.y = AABB.max.y;
		subspace1.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace1.max.z = AABB.max.z;
		pNode = proot->CreateSpace(code + std::to_string(1), subspace1);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// subspace 2
		subspace2.min.x = AABB.min.x;
		subspace2.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace2.min.y = AABB.min.y;
		subspace2.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace2.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace2.max.z = AABB.max.z;
		pNode = proot->CreateSpace(code + std::to_string(2), subspace2);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// subspace 3
		subspace3.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace3.max.x = AABB.max.x;
		subspace3.min.y = AABB.min.y;
		subspace3.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace3.min.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		subspace3.max.z = AABB.max.z;
		pNode = proot->CreateSpace(code + std::to_string(3), subspace3);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// Downspace
		// subspace 4
		subspace4.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace4.max.x = AABB.max.x;
		subspace4.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace4.max.y = AABB.max.y;
		subspace4.min.z = AABB.min.z;
		subspace4.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		pNode = proot->CreateSpace(code + std::to_string(4), subspace4);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// subspace 5
		subspace5.min.x = AABB.min.x;
		subspace5.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace5.min.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace5.max.y = AABB.max.y;
		subspace5.min.z = AABB.min.z;
		subspace5.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		pNode = proot->CreateSpace(code + std::to_string(5), subspace5);
		pNode->m_rootPtr = proot;

		// subspace 6
		subspace6.min.x = AABB.min.x;
		subspace6.max.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace6.min.y = AABB.min.y;
		subspace6.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace6.min.z = AABB.min.z;
		subspace6.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		pNode = proot->CreateSpace(code + std::to_string(6), subspace6);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

		// subspace 7
		subspace3.min.x = AABB.max.x - (AABB.max.x - AABB.min.x) / 2;
		subspace3.max.x = AABB.max.x;
		subspace3.min.y = AABB.min.y;
		subspace3.max.y = AABB.max.y - (AABB.max.y - AABB.min.y) / 2;
		subspace3.min.z = AABB.min.z;
		subspace3.max.z = AABB.max.z - (AABB.max.z - AABB.min.z) / 2;
		pNode = proot->CreateSpace(code + std::to_string(7), subspace7);
		pNode->m_rootPtr = proot;
		pNode->parent = this;

	}

	bool OctreeNode::FindNode(std::shared_ptr<Object3D> pTarget, std::vector<OctreeNode*> &nodes) noexcept
	{
		if (AABB.isIntersected(pTarget->GetAABB()))
		{
			if (isLeaf)
			{
				for(auto pObject : Objects)
					if (pTarget == pObject.second)
					{
						nodes.push_back(this);
						return true;
					}

				return false;
			}
			else
			{
				bool ret = false;
				for (std::size_t k = 0; k < 8; k++)
				{
					assert(m_rootPtr);
					auto pNode = m_rootPtr->FindNode(code + std::to_string(k));
					ret = ret || pNode->FindNode(pTarget, nodes);
				}

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

		if (isLeaf)
		{
			for (auto pObject : Objects)
				if (pObject.second->GetAABB().isIntersected(_AABB))
					objects.push_back(pObject.second);
		}
		else
		{
			assert(m_rootPtr);
			for (std::size_t k = 0; k < 8; k++)
			{
				auto pNode = m_rootPtr->FindNode(code + std::to_string(k));
				pNode->FindObjects(_AABB,objects);
			}
		}
	}

	bool OctreeNode::isNorth(char ch)
	{
		return ch == '0' || ch == '1' 
			|| ch == '4' || ch == '5';
	}

	bool OctreeNode::isSouth(char ch)
	{
		return ch == '2' || ch == '3'
			|| ch == '6' || ch == '7';
	}

	bool OctreeNode::isEast(char ch)
	{
		return ch == '1' || ch == '3'
			|| ch == '5' || ch == '7';
	}

	bool OctreeNode::isWest(char ch)
	{
		return ch == '0' || ch == '2'
			|| ch == '4' || ch == '6';
	}

	bool OctreeNode::isUp(char ch)
	{
		return ch == '0' || ch == '1'
			|| ch == '2' || ch == '3';
	}

	bool OctreeNode::isDown(char ch)
	{
		return ch == '4' || ch == '5'
			|| ch == '6' || ch == '7';
	}

	bool OctreeNode::isBoundaryNode() const noexcept
	{
		for (auto ch : code)
		{
			if (!OctreeNode::isNorth(ch) && !OctreeNode::isSouth(ch) &&
				!OctreeNode::isEast(ch) && !OctreeNode::isWest(ch) &&
				!OctreeNode::isUp(ch) && !OctreeNode::isDown(ch))
				return false;
		}

		return true;
	}

	std::string OctreeNode::GetNorthCode() const noexcept
	{
		std::string ret = code;
		if ((OctreeNode::isNorth(ret.back())))
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isNorth(ret[k]))
					ret[k] += 2;
				else
				{
					ret[k] -= 2;
					break;
				}
			}
		}
		else if (OctreeNode::isSouth(ret.back()))
			ret[ret.size() - 1] -= 2;

		return ret;
	}

	std::string OctreeNode::GetSouthCode() const noexcept
	{
		std::string ret = code;
		if (OctreeNode::isSouth(ret.back()))
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isSouth(ret[k]))
					ret[k] -= 2;
				else
				{
					ret[k] += 2;
					break;
				}
			}
		}
		else if (OctreeNode::isNorth(ret.back()))
			ret[ret.size() - 1] += 2;

		return ret;
	}

	std::string OctreeNode::GetEastCode() const noexcept
	{
		std::string ret = code;
		if (OctreeNode::isEast(ret.back()))
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isEast(ret[k]))
					ret[k] -= 1;
				else
				{
					ret[k] += 1;
					break;
				}
			}
		}
		else if (OctreeNode::isWest(ret.back()))
			ret[ret.size() - 1] += 1;

		return ret;
	}

	std::string OctreeNode::GetWestCode() const noexcept
	{
		std::string ret = code;
		if (OctreeNode::isWest(ret.back()))
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isWest(ret[k]))
					ret[k] += 1;
				else
				{
					ret[k] -= 1;
					break;
				}
			}
		}
		else if (OctreeNode::isEast(ret.back()))
			ret[ret.size() - 1] -= 1;

		return ret;
	}

	std::string OctreeNode::GetUpCode() const noexcept
	{
		std::string ret = code;
		if (OctreeNode::isDown(ret.back()))
			ret[ret.size() - 1] += 4;
		else
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isUp(ret[k]))
					ret[k] += 4;
				else {
					ret[k] -= 4;
					break;
				}
			}
		}

		return ret;
	}

	std::string OctreeNode::GetDownCode() const noexcept
	{
		std::string ret = code;
		if (OctreeNode::isUp(ret.back()))
			ret[ret.size() - 1] -= 4;
		else
		{
			for (std::size_t k = ret.size() - 1; k >= 0; k++)
			{
				if (OctreeNode::isDown(ret[k]))
					ret[k] -= 4;
				else
				{
					ret[k] += 4;
					break;
				}
			}
		}

		return ret;
	}

	void OctreeNode::MoveAndAdjust(const std::string &name,int direction)
	{
		auto it = Objects.find(name);
		if (it == Objects.end())
			return;

		std::string newCode;
		auto pObject = it->second;

		switch (direction)
		{
		case MOVE_NORTH :
			newCode = GetNorthCode();
			break;
		case MOVE_SOUTH:
			newCode = GetSouthCode();
			break;
		case MOVE_EAST:
			newCode = GetEastCode();
			break;
		case MOVE_WEST:
			newCode = GetWestCode();
			break;
		case MOVE_UP:
			newCode = GetUpCode();
			break;
		case MOVE_DOWN:
			newCode = GetDownCode();
			break;
		default:
			return;
		}

		auto proot = m_rootPtr;

		assert(proot);
		
		auto pNode = proot->FindNode(newCode);

		if (!pNode)
		{
			// Low level move into high level
			newCode.pop_back();
			while (!proot->FindNode(newCode))
				newCode.pop_back();

			// Insert into next node
			auto pTargetNode = proot->FindNode(newCode);
			if(pObject->AABB.isIntersected(pTargetNode->AABB) &&
				pObject->m_OctreePtrs.find(pTargetNode->code) != pObject->m_OctreePtrs.end())
				pTargetNode->InsertObject(pObject);

			// Remove from the current node when their AABBs do not intersect
			if (!AABB.isIntersected(pObject->AABB))
			{
				Objects.erase(Objects.find(pObject->GetObjectName()));
				pObject->m_OctreePtrs.erase(pObject->m_OctreePtrs.find(code));

				// Merge space if the number of objects less than PARTITION_MIN_NODENUM
				if (parent->GetObjNum() < PARTITION_MIN_NODENUM)
					parent->SpaceMerge();
				
			}
		}
		else
		{
			// High level move into low level or equal level
			// Insert into next node
			if (pNode->AABB.isIntersected(pObject->AABB) &&
				pObject->m_OctreePtrs.find(pNode->code) != pObject->m_OctreePtrs.end())
				pNode->InsertObject(pObject);

			// Remove from the current node when their AABBs do not intersect
			if (!AABB.isIntersected(pObject->AABB))
			{
				Objects.erase(Objects.find(pObject->GetObjectName()));
				pObject->m_OctreePtrs.erase(pObject->m_OctreePtrs.find(code));

				// Merge space if the number of objects less than PARTITION_MIN_NODENUM
				if (parent->GetObjNum() < PARTITION_MIN_NODENUM)
					parent->SpaceMerge();
			}
		}
	}

	int OctreeNode::GetObjNum() const noexcept {
		auto sum = 0;

		for (std::size_t k = 0; k < 8; k++)
		{
			auto pNode = m_rootPtr->FindNode(code + std::to_string(k));
			if (pNode)
			{
				if (pNode->isLeaf)
					sum += pNode->Objects.size();
				else
					sum += pNode->GetObjNum();
			}
		}

		return sum;
	}

	void OctreeRoot::RemoveNode(const std::string &code) noexcept
	{
		auto it = Nodes.find(code);
		if (it != Nodes.end())
			Nodes.erase(it);
	}

	void OctreeNode::SpaceMerge() noexcept
	{
		for (std::size_t k = 0; k < 8; k++)
		{
			auto pNode = m_rootPtr->FindNode(code + std::to_string(k));
			if (pNode)
			{
				for (auto piter = pNode->Objects.begin(); piter != pNode->Objects.end();)
				{
					Objects.insert(std::make_pair(piter->first,piter->second));
					piter->second->m_OctreePtrs.erase(pNode->code);
					piter->second->m_OctreePtrs.insert(code);
					pNode->Objects.erase(piter->second->GetObjectName());
				}

				m_rootPtr->RemoveNode(pNode->code);
			}
		}

		isLeaf = true;
	}

	OctreeRoot::OctreeRoot(const glm::vec3 &center,const CXCRect3 &size)
		: OctreeNode(size)
	{
		m_rootPtr = this;
		parent = nullptr;
		code = "";
		SpacePartition();
		isLeaf = false;
	}

	OctreeRoot::~OctreeRoot()
	{

	}

	std::shared_ptr<OctreeNode> OctreeRoot::FindNode(const std::string &code) noexcept
	{
		auto it = Nodes.find(code);
		if (it != Nodes.end())
			return it->second;
		else
			return nullptr;
	}

	std::shared_ptr<OctreeNode> OctreeRoot::CreateSpace(const std::string &code, const CXCRect3 &AABB) noexcept
	{
		auto pNode = std::make_shared<OctreeNode>(AABB);
		pNode->code = code;
		Nodes.insert(std::make_pair(code, pNode));

		return pNode;
	}
}
