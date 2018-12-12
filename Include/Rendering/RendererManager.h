#include "General/DefineTypes.h"
#include "Utilities/Singleton.inl"
#include "Scene/Lighting.h"
#include "Rendering/MeshRenderer.h"

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

namespace cxc {

	class RendererManager final : public Singleton<RendererManager>
	{
	
	public:
		friend Singleton<RendererManager>;

		explicit RendererManager();
		~RendererManager();

		RendererManager(const RendererManager &) = delete;
		RendererManager(const RendererManager &&) = delete;

		RendererManager& operator=(const RendererManager &) = delete;
		RendererManager& operator=(const RendererManager &&) = delete;

	public:

		void AddRender( std::shared_ptr<MeshRenderer> pRender) noexcept;
		void DeleteRender(const std::string &name) noexcept;

		std::shared_ptr<MeshRenderer> GetRenderPtr(const std::string &name) noexcept;
		void UseRender(const std::string& RenderName);
		std::shared_ptr<MeshRenderer> GetCurrentUsedRender() { return CurrentUsedRender; }
		void SetCurrentUsedRender(std::shared_ptr<MeshRenderer> pRender);

	public:

		std::shared_ptr<Shader> GetShader(const std::string& ShaderName);
		void AddShader(std::shared_ptr<Shader> pShader);
		std::shared_ptr<Shader> FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName);

	private:

		std::shared_ptr<MeshRenderer> CurrentUsedRender;

		std::unordered_map<std::string, std::shared_ptr<MeshRenderer>> RendersMap;

		std::unordered_map<std::string, std::shared_ptr<Shader>> pShadersMap;
	};
}

#endif  // CXC_RENDERERMANAGER_H
