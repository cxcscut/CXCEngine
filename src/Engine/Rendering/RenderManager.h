#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Utilities\Singleton.inl"
#include "..\Rendering\Lighting.h"
#include "..\Rendering\MeshRender.h"

#else

#include "../General/DefineTypes.h"
#include "../Utilities/Singleton.inl"
#include "../Rendering/Lighting.h"
#include "../Rendering/MeshRender.h"

#endif // WIN32

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

	private:

		std::shared_ptr<MeshRender> CurrentUsedRender;

		std::unordered_map<std::string, std::shared_ptr<MeshRender>> m_Renders;
	};
}

#endif  // CXC_RENDERERMANAGER_H
