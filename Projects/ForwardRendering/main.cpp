#include "CXCEngine.h"
#include "Rendering/ForwardRenderer.h"

using namespace cxc;

// Forward render shaders
static const std::string ForwardRenderVSPath = "G:\\cxcengine\\Src\\GLSL\\ForwardRendering\\ForwardRenderVS.glsl";
static const std::string ForwardRenderFSPath = "G:\\cxcengine\\Src\\GLSL\\ForwardRendering\\ForwardRenderFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\Projects\\Models\\EN_Building_H_03.FBX";
static const std::string HumanoidScene = "G:\\cxcengine\\Projects\\Models\\humanoid.fbx";

std::shared_ptr<SubMeshRenderer> CreateForwardRender();
void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CObject>>& Objects);
std::vector<std::shared_ptr<CObject>> CreateObjects(const std::vector<std::shared_ptr<Mesh>>& Meshes);

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

	auto pRendererMgr = RendererManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderer = CreateForwardRender();
	pRendererMgr->AddRenderer(pRenderer);

	pSceneManager->AddCamera("MainCamera", CameraPos, CameraOrigin, CameraUpVector, ProjectionMatrix);
	pSceneManager->SetCameraActive("MainCamera");

	std::vector<std::shared_ptr<Mesh>> OutMeshes;
	bool bResult = pSceneManager->LoadSceneFromFBX(SceneFBXFile, OutMeshes);
	auto Objects = CreateObjects(OutMeshes);
	if (bResult)
	{
		BindSubMeshRenderer(pRenderer, Objects);
	}

	for (auto Object : Objects)
	{
		pSceneManager->AddObject(Object);
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::vector<std::shared_ptr<CObject>> CreateObjects(const std::vector<std::shared_ptr<Mesh>>& Meshes)
{
	auto pSceneManager = SceneManager::GetInstance();
	std::vector<std::shared_ptr<CObject>> RetObjects;

	for (auto pMesh : Meshes)
	{
		auto Object = NewObject<CObject>(pMesh->GetMeshName());
		auto StaticMeshComponent = NewObject<CStaticMeshComponent>(pMesh);
		Object->AttachComponent<CStaticMeshComponent>(StaticMeshComponent);
		RetObjects.push_back(Object);
	}

	return RetObjects;
}

void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CObject>>& Objects)
{
	auto pRendererMgr = RendererManager::GetInstance();

	for (auto Object : Objects)
	{
		auto StaticMeshComponent = Object->GetComponent<CStaticMeshComponent>();
		if (StaticMeshComponent)
		{
			size_t SubMeshCount = StaticMeshComponent->GetStaticMesh()->GetSubMeshCount();
			for (size_t Index = 0; Index < SubMeshCount; ++Index)
			{
				auto pSubMesh = StaticMeshComponent->GetStaticMesh()->GetSubMesh(Index);
				pRendererMgr->BindSubMeshRenderer(pSubMesh, pRenderer);
			}
		}
	}
}

std::shared_ptr<SubMeshRenderer> CreateForwardRender()
{
	auto pRendererMgr = RendererManager::GetInstance();

	// Foward Phong render with no shadows
	auto ForwardPhongVS = pRendererMgr->FactoryShader("ForwardVS", eShaderType::VERTEX_SHADER, ForwardRenderVSPath);
	auto ForwardPhongFS = pRendererMgr->FactoryShader("ForwardFS", eShaderType::FRAGMENT_SHADER, ForwardRenderFSPath);

	auto ForwardPhongPipeline = NewObject<ForwardRenderPipeline>("ForwardPhongPipeline");
	ForwardPhongPipeline->AttachShader(ForwardPhongVS);
	ForwardPhongPipeline->AttachShader(ForwardPhongFS);

	auto PhongRender = NewObject<ForwardRenderer>("ForwardPhongRender");
	PhongRender->AddPipeline(ForwardPhongPipeline);
	PhongRender->InitializeRenderer();

	return PhongRender;
}
