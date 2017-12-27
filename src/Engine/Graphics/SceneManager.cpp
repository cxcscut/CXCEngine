#include "SceneManager.h"
#include "..\EngineFacade\EngineFacade.h"

namespace cxc {

	// Collision detection callback function
	void SceneManager::nearCallback(void *data, dGeomID o1, dGeomID o2) noexcept
	{

		int num;

		dContact contacts[MAX_CONTACT_NUM];

		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact)) return;

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

				dJointID joint = dJointCreateContact(pSceneMgr->m_WorldID, pSceneMgr->m_ContactJoints, contacts + i);
				dJointAttach(joint, dGeomGetBody(o1), dGeomGetBody(o2));
			}
		}
	}

	SceneManager::SceneManager()
		: m_ObjectMap(),TotalIndicesNum(0U), m_LightPos(glm::vec3(0, 1500, 1500)),
		m_TopLevelSpace(0),m_WorldID(0), m_ContactJoints(0)
	{
		m_pTextureMgr = TextureManager::GetInstance();
		m_pCamera = std::make_shared<Camera>();
		m_pRendererMgr = RendererManager::GetInstance();

	}

	SceneManager::~SceneManager()
	{
		m_ObjectMap.clear();

		// Destroy world
		if(m_WorldID)
			dWorldDestroy(m_WorldID);
		
		// Destroy joint group
		if(m_ContactJoints)
			dJointGroupDestroy(m_ContactJoints);
		
		// Destroy space
		if(m_TopLevelSpace)
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

	void SceneManager::InitializePhysicalObjects() noexcept
	{
		for (auto object : m_ObjectMap) {
			object.second->InitializeRigidBodies(m_WorldID,m_TopLevelSpace);
		}
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

	}

	void SceneManager::DrawScene() noexcept
	{
		for (auto pObject : m_ObjectMap)
			if(pObject.second->isEnable())
				pObject.second->DrawObject();
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

}
