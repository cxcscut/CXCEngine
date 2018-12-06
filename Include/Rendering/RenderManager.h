#include "General/DefineTypes.h"
#include "Utilities/Singleton.inl"
#include "Scene/Lighting.h"
#include "Rendering/MeshRender.h"

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

namespace cxc {

	class RenderManager final : public Singleton<RenderManager>
	{
	
	public:
		friend Singleton<RenderManager>;

		explicit RenderManager();
		~RenderManager();

		RenderManager(const RenderManager &) = delete;
		RenderManager(const RenderManager &&) = delete;

		RenderManager& operator=(const RenderManager &) = delete;
		RenderManager& operator=(const RenderManager &&) = delete;

	public:

		void AddRender( std::shared_ptr<MeshRender> pRender) noexcept;
		void DeleteRender(const std::string &name) noexcept;

		std::shared_ptr<MeshRender> GetRenderPtr(const std::string &name) noexcept;
		void UseRender(const std::string& RenderName);
		std::shared_ptr<MeshRender> GetCurrentUsedRender() { return CurrentUsedRender; }
		void SetCurrentUsedRender(std::shared_ptr<MeshRender> pRender);

	public:

		std::shared_ptr<Shader> GetShader(const std::string& ShaderName);
		void AddShader(std::shared_ptr<Shader> pShader);
		std::shared_ptr<Shader> FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName);

	private:

		std::shared_ptr<MeshRender> CurrentUsedRender;

		std::unordered_map<std::string, std::shared_ptr<MeshRender>> RendersMap;

		std::unordered_map<std::string, std::shared_ptr<Shader>> pShadersMap;
	};
}

#endif  // CXC_RENDERERMANAGER_H
