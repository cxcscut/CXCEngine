#include "CXCEngine.h"
#include "Rendering/ForwardRender.h"
#include "Rendering/ShadowRender.h"
#include "Rendering/ShadowRenderPipeline.h"
#include "Rendering/DeferredRender.h"
#include "Rendering/DeferredRenderPipeline.h"

using namespace cxc;

// Deferred render shaders
static const std::string DeferredRenderVSPath = "G:\\cxcengine\\Src\\GLSL\\DeferredRendering\\DeferredRenderVS.glsl";
static const std::string DeferredRenderFSPath = "G:\\cxcengine\\Src\\GLSL\\DeferredRendering\\DeferredRenderFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\Projects\\Models\\EN_Building_H_03.FBX";

std::shared_ptr<MeshRender> CreateDeferredRender();

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

	GEngine::SetCamera(CameraPos, CameraOrigin, CameraUpVector, ProjectionMatrix);
	auto pRenderMgr = RenderManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRender = CreateDeferredRender();
	pRenderMgr->AddRender(pRender);

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
				GEngine::BindMeshRender(pRender, pObject.second, i);
			}
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::shared_ptr<MeshRender> CreateDeferredRender()
{
	auto pRenderMgr = RenderManager::GetInstance();

	auto pDeferredRender = NewObject<DeferredRender>("DeferredRender");
	auto pDeferredRenderPipeline = NewObject<DeferredRenderPipeline>();

	auto DeferredRenderVS = pRenderMgr->FactoryShader("DeferredShaderVS", eShaderType::VERTEX_SHADER, DeferredRenderVSPath);
	auto DeferredRenderFS = pRenderMgr->FactoryShader("DeferredShaderFS", eShaderType::FRAGMENT_SHADER, DeferredRenderFSPath);
	pDeferredRenderPipeline->AttachShader(DeferredRenderVS);
	pDeferredRenderPipeline->AttachShader(DeferredRenderFS);

	pDeferredRender->SetDeferredRenderPipeline(pDeferredRenderPipeline);
	pDeferredRender->InitializeRender();

	return pDeferredRender;
}