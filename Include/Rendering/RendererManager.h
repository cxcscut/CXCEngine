#include "General/DefineTypes.h"
#include "Utilities/Singleton.inl"
#include "Scene/Lighting.h"
#include "Rendering/SubMeshRenderer.h"

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

namespace cxc {

	class RendererContext;

	enum class eShadingMode : uint32_t 
	{
		SurfaceMode = 0,
		WireframeMode = 1
	};

	class RendererManager final : public Singleton<RendererManager>
	{
	
	public:
		friend Singleton<RendererManager>;
		friend class SceneManager;

		explicit RendererManager();
		~RendererManager();

		RendererManager(const RendererManager &) = delete;
		RendererManager(const RendererManager &&) = delete;

		RendererManager& operator=(const RendererManager &) = delete;
		RendererManager& operator=(const RendererManager &&) = delete;

	public:

		void BindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer);
		void UnBindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer);
		void AddRenderer( std::shared_ptr<SubMeshRenderer> pRenderer) noexcept;
		void DeleteRenderer(const std::string &name) noexcept;

		std::shared_ptr<SubMeshRenderer> GetRendererPtr(const std::string &name) noexcept;
		void UseRenderer(std::shared_ptr<SubMeshRenderer> pRenderer);
		void UseRenderer(const std::string& RenderName);
		std::shared_ptr<SubMeshRenderer> GetCurrentUsedRenderer() { return CurrentUsedRenderer; }
		void SetCurrentUsedRenderer(std::shared_ptr<SubMeshRenderer> pRenderer);

	public:

		std::shared_ptr<Shader> GetShader(const std::string& ShaderName);
		void AddShader(std::shared_ptr<Shader> pShader);
		std::shared_ptr<Shader> FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName);

	private:

		void AddSubMeshToRendererContext(std::shared_ptr<SubMesh> pSubMesh);
		void ClearRendererContext();

	private:

		std::unordered_map<std::shared_ptr<SubMesh>, std::shared_ptr<SubMeshRenderer>> SubMeshRendererBindings;

		std::unordered_map<std::string, std::shared_ptr<RendererContext>> RendererContextsMap;

		std::shared_ptr<SubMeshRenderer> CurrentUsedRenderer;

		std::unordered_map<std::string, std::shared_ptr<SubMeshRenderer>> RenderersMap;

		std::unordered_map<std::string, std::shared_ptr<Shader>> pShadersMap;
	};
}

#endif  // CXC_RENDERERMANAGER_H
