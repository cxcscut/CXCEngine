#include "Rendering/RendererManager.h"
#include "Rendering/ForwardRenderer.h"
#include "Rendering/DebugMeshRenderPipeline.h"
#include "Rendering/RendererContext.h"
#include "Geometry/Mesh.h"
#include "Geometry/DebugMesh.h"
#include "Utilities/DebugLogger.h"
#include "Geometry/SubMesh.h"

const std::string DebugMeshVSSourceCode = "#version 430 core \n" \
"layout(location = 0) in vec3 vertexPosition_modelspace;\n " \
"uniform mat4 P;\n" \
"uniform mat4 V;\n" \
"uniform mat4 M;\n" \
"mat4 MVP = P * V * M;\n"\
"void main()\n" \
"{\n" \
"gl_Position = MVP * vec4(vertexPosition_modelspace, 1);\n" \
"}\n";

const std::string DebugMeshFSSourceCode = "#version 430 core\n" \
"out vec3 OutColor;\n"\
"uniform vec3 VertexColor;\n" \
"void main()\n" \
"{\n" \
"OutColor = VertexColor;\n"\
"}\n";

namespace cxc 
{
	
	RendererManager::RendererManager()
	{
	}

	RendererManager::~RendererManager()
	{
		
	}

	void RendererManager::BindDebugMesh(std::shared_ptr<DebugMesh> pMesh)
	{
		if (pMesh)
		{
			auto DebugRendererIter = RenderersMap.find("DebugMeshRenderer");
			if (DebugRendererIter != RenderersMap.end())
			{
				BindMeshRenderer(pMesh, DebugRendererIter->second);
			}
		}
	}

	void RendererManager::CreateEngineDefaultRenderer()
	{
		// Create engine defalut renderer
		// Create debug mesh renderer and pipeline
		auto DebugMeshVS = NewObject<Shader>();
		auto DebugMeshFS = NewObject<Shader>();

		DebugMeshVS->ShaderName = "DebugMeshVS"; DebugMeshVS->ShaderType = eShaderType::VERTEX_SHADER;
		DebugMeshFS->ShaderName = "DebugMeshFS"; DebugMeshFS->ShaderType = eShaderType::FRAGMENT_SHADER;

		std::string CompileLog;
		bool bResult = DebugMeshVS->CompileShader(DebugMeshVSSourceCode, CompileLog);
		if (!bResult)
			DEBUG_LOG(eLogType::Error, "RendererManager::CreateEngineDefaultRenderer, Failed to compile debug mesh vertex shader, OutLog : " + CompileLog + '\n');

		bResult = DebugMeshFS->CompileShader(DebugMeshFSSourceCode, CompileLog);
		if (!bResult)
			DEBUG_LOG(eLogType::Error, "RendererManager::CreateEngineDefaultRenderer, Failed to compile debug mesh fragment shader, OutLog : " + CompileLog + '\n');

		auto DebugMeshPipeline = NewObject<DebugMeshRenderPipeline>();
		DebugMeshPipeline->AttachShader(DebugMeshVS);
		DebugMeshPipeline->AttachShader(DebugMeshFS);

		auto DebugMeshRenderer = NewObject<ForwardRenderer>("DebugMeshRenderer");
		DebugMeshRenderer->PushPipeline(DebugMeshPipeline);
		DebugMeshRenderer->InitializeRenderer();

		AddRenderer(DebugMeshRenderer);
	}

	void RendererManager::SetCurrentUsedRenderer(std::shared_ptr<SubMeshRenderer> pRenderer)
	{
		CurrentUsedRenderer = pRenderer;
	}

	std::shared_ptr<Shader> RendererManager::FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName)
	{
		auto pNewShader = NewObject<Shader>(ShaderName, ShaderFileName, ShaderType);
		if (pNewShader && pNewShader->CompileShader())
		{
			pNewShader->ShaderFileName = ShaderFileName;
			AddShader(pNewShader);

			return pNewShader;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Shader>RendererManager::GetShader(const std::string& ShaderName)
	{
		auto iter = pShadersMap.find(ShaderName);
		if (iter != pShadersMap.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void RendererManager::AddShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader)
		{
			pShadersMap.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<SubMeshRenderer> RendererManager::GetRendererPtr(const std::string &name) noexcept
	{
		auto it = RenderersMap.find(name);
		if (it != RenderersMap.end())
			return it->second;
		else
			return nullptr;

	}

	void RendererManager::UseRenderer(std::shared_ptr<SubMeshRenderer> pRenderer)
	{
		if (pRenderer)
			CurrentUsedRenderer = pRenderer;
	}

	void RendererManager::UseRenderer(const std::string& RenderName)
	{
		auto pRenderer = GetRendererPtr(RenderName);
		if (pRenderer)
		{
			CurrentUsedRenderer = pRenderer;
		}
	}

	void RendererManager::AddRenderer(std::shared_ptr<SubMeshRenderer> pRenderer) noexcept
	{
		if (pRenderer)
		{
			RenderersMap.insert(std::make_pair(pRenderer->GetRendererName(), pRenderer));
		}
	}

	void RendererManager::DeleteRenderer(const std::string &name) noexcept
	{
		auto it = RenderersMap.find(name);
		if (it != RenderersMap.end())
			RenderersMap.erase(it);
	}

	void RendererManager::AddLightToRendererContext(std::vector<std::shared_ptr<LightSource>> Lights, std::shared_ptr<SubMesh> pSubMesh)
	{
		auto RendererIter = SubMeshRendererBindings.find(pSubMesh);
		if (RendererIter != SubMeshRendererBindings.end())
		{
			auto pRenderer = RendererIter->second;
			auto ContextIter = RendererContextsMap.find(pRenderer->GetRendererName());
			if (ContextIter != RendererContextsMap.end())
			{
				ContextIter->second->SetShadingLights(Lights);
			}
		}
	}

	void RendererManager::AddSubMeshToRendererContext(std::shared_ptr<SubMesh> pSubMesh)
	{
		auto RendererBindingIter = SubMeshRendererBindings.find(pSubMesh);
		if (RendererBindingIter == SubMeshRendererBindings.end())
			return;

		auto pSubMeshRenderer = RendererBindingIter->second;
		auto SubRendererIter = RendererContextsMap.find(pSubMeshRenderer->GetRendererName());
		if (SubRendererIter != RendererContextsMap.end())
		{
			auto Context = SubRendererIter->second;
			Context->AddBindedSubMesh(pSubMesh);
		}
		else
		{
			auto Context = NewObject<RendererContext>(pSubMeshRenderer);
			Context->AddBindedSubMesh(pSubMesh);
			RendererContextsMap.insert(std::make_pair(pSubMeshRenderer->GetRendererName(), Context));
		}
	}

	void RendererManager::ClearRendererContext()
	{
		RendererContextsMap.clear();
	}

	void RendererManager::BindMeshRenderer(std::shared_ptr<Mesh> pMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer)
	{
		if (pMesh)
		{
			size_t SubMeshCount = pMesh->GetSubMeshCount();
			for (size_t Index = 0; Index < SubMeshCount; ++Index)
			{
				auto pSubMesh = pMesh->GetSubMesh(Index);
				BindSubMeshRenderer(pSubMesh, pSubMeshRenderer);
			}
		}
	}

	void RendererManager::BindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer)
	{
		if (pSubMesh)
		{
			SubMeshRendererBindings.insert(std::make_pair(pSubMesh, pSubMeshRenderer));
		}
	}

	void RendererManager::UnBindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer)
	{
		auto Iter = SubMeshRendererBindings.find(pSubMesh);
		if (Iter != SubMeshRendererBindings.end())
		{
			SubMeshRendererBindings.erase(Iter);
		}
	}
}