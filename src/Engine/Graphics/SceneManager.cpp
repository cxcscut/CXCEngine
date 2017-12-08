#include "SceneManager.h"
#include "..\EngineFacade\EngineFacade.h"

namespace cxc {

	SceneManager::SceneManager()
		: m_ObjectMap(),TotalIndicesNum(0U), m_LightPos(glm::vec3(0, 1500, 1500)),
		m_TopLevelSpace(0)
	{
		m_pTextureMgr = TextureManager::GetInstance();
		m_pCamera = std::make_shared<Camera>();
		m_pRendererMgr = RendererManager::GetInstance();

	}

	SceneManager::~SceneManager()
	{
		m_ObjectMap.clear();

		// Destroy world
		dWorldDestroy(m_WorldID);
		
		// Destroy joint group
		dJointGroupDestroy(m_ContactJoints);
		
		// Destroy space
		dSpaceDestroy(m_TopLevelSpace);

		// Deallocation for extra memory of ODE runtime
		dCloseODE();
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

	void SceneManager::CreatePhysicalWorld(const glm::vec3 &gravity) noexcept
	{
		// Create world
		m_WorldID = dWorldCreate();

		// Setting parameters of world
		dWorldSetERP(m_WorldID, 0.2);
		dWorldSetCFM(m_WorldID, 1e-5);

		// Create top-level space
		dHashSpaceSetLevels(m_TopLevelSpace,0,5);
		
		// Set gravity
		dWorldSetGravity(m_WorldID,gravity.x,gravity.y,gravity.z);

		// Create rigid bodies and colliders
		InitializePhysicalObjects();

		// Create joint gruop for contact joint
		m_ContactJoints = dJointGroupCreate(0);
	}

	void SceneManager::InitializePhysicalObjects() noexcept
	{
		for (auto object : m_ObjectMap) {
			object.second->InitializeRigidBodies(m_WorldID);
			object.second->AttachCollider(m_TopLevelSpace);
		}
	}

	void SceneManager::PhysicalLoop() noexcept
	{
		dSpaceCollide(m_TopLevelSpace,0,&nearCallback);

		dWorldQuickStep(m_WorldID, WOLRD_QUICK_STEPSIZE);

		dJointGroupEmpty(m_ContactJoints);
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
		SetCameraParams(glm::vec3(0, 2000, 2000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
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

	GLboolean SceneManager::LoadSceneFromFBX(const std::string &scenefile, const std::string &scene_name) noexcept
	{
		return GL_TRUE;
	}

	void SceneManager::AddObject(const std::string &ObjectName, const std::shared_ptr<Object3D > &ObjectPtr) noexcept
	{
		m_ObjectMap.insert(std::make_pair(ObjectName, ObjectPtr));
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

	void SceneManager::SynchronizeWorld() noexcept
	{

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
