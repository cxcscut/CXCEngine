#include "CXCEngine.h"
#include "Rendering/ForwardRenderer.h"

using namespace cxc;

// Forward render shaders
static const std::string ForwardRenderVSPath = "G:\\cxcengine\\Src\\GLSL\\ForwardRendering\\ForwardRenderVS.glsl";
static const std::string ForwardRenderFSPath = "G:\\cxcengine\\Src\\GLSL\\ForwardRendering\\ForwardRenderFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\Projects\\Models\\EN_Building_H_03.FBX";
static const std::string HumanoidScene = "G:\\cxcengine\\Projects\\Models\\humanoid.fbx";

std::shared_ptr<MeshRenderer> CreateForwardRender();

int main()
{
	glm::vec3 CameraPos = glm::vec3(50, 50, 50);
	glm::vec3 CameraOrigin = glm::vec3(0, 0, 0);
	glm::vec3 CameraUpVector = glm::vec3(0, 0, 1);
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);

	DisplayParameters DisplayConf;
	DisplayConf.ApplicationTitle = "Powered by CXCEngine";
	DisplayConf.bIsDecorated = true;
	DisplayConf.WindowHeight = 600;
	DisplayConf.WindowWidth = 800;
	DisplayConf.WindowPosX = 200;
	DisplayConf.WindowPosY = 200;
	GEngine::ConfigureEngineDisplaySettings(DisplayConf);
	GEngine::InitializeEngine();

	auto pRenderMgr = RendererManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pForwardRender = CreateForwardRender();
	pRenderMgr->AddRender(pForwardRender);

	pSceneManager->AddCamera("MainCamera", CameraPos, CameraOrigin, CameraUpVector, ProjectionMatrix);
	pSceneManager->SetCameraActive("MainCamera");

	bool bResult = pSceneManager->LoadSceneFromFBX(SceneFBXFile);
	if (bResult)
	{
		// Bind mesh render
		auto ObjectMap = pSceneManager->GetObjectMap();
		for (auto pObject : ObjectMap)
		{
			auto MeshCount = pObject.second->GetMeshCount();
			for (size_t i = 0; i < MeshCount; ++i)
			{
				GEngine::BindMeshRender(pForwardRender, pObject.second, i);
			}
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::shared_ptr<MeshRenderer> CreateForwardRender()
{
	auto pRenderMgr = RendererManager::GetInstance();

	// Foward Phong render with no shadows
	auto ForwardPhongVS = pRenderMgr->FactoryShader("ForwardVS", eShaderType::VERTEX_SHADER, ForwardRenderVSPath);
	auto ForwardPhongFS = pRenderMgr->FactoryShader("ForwardFS", eShaderType::FRAGMENT_SHADER, ForwardRenderFSPath);

	auto ForwardPhongPipeline = NewObject<ForwardRenderPipeline>("ForwardPhongPipeline");
	ForwardPhongPipeline->AttachShader(ForwardPhongVS);
	ForwardPhongPipeline->AttachShader(ForwardPhongFS);

	auto PhongRender = NewObject<ForwardRenderer>("ForwardPhongRender");
	PhongRender->SetForwardRenderPipeline(ForwardPhongPipeline);
	PhongRender->InitializeRender();

	return PhongRender;
}
