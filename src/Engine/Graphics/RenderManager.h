#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\inl\Singleton.inl"
#include "..\Graphics\Lighting.h"
#include "..\Graphics\Render.h"

#else

#include "../General/DefineTypes.h"
#include "../inl/Singleton.inl"
#include "../Graphics/Lighting.h"
#include "../Graphics/Render.h"

#endif // WIN32

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

namespace cxc {

	class RenderManager final : public Singleton<RenderManager>
	{
		using CubeMapCameraPose = struct CubeMapCameraPose {
			GLenum CubeMapFace;
			glm::vec3 Direction;
			glm::vec3 UpVector;
		};

	public:
		friend Singleton<RenderManager>;

		explicit RenderManager();
		~RenderManager();

		RenderManager(const RenderManager &) = delete;
		RenderManager(const RenderManager &&) = delete;

		RenderManager& operator=(const RenderManager &) = delete;
		RenderManager& operator=(const RenderManager &&) = delete;

	public:

		void addRender(const std::string &name, std::shared_ptr<Render> pRender) noexcept;
		void deleteRender(const std::string &name) noexcept;
		std::shared_ptr<Render> GetRenderPtr(const std::string &name) noexcept;
		void UseRender(const std::string& RenderName);
		std::shared_ptr<Render> GetCurrentUsedRender() { return CurrentUsedRender; }
		void SetCurrentUsedRender(std::shared_ptr<Render> pRender);

		// Shadowmap
	public:

		GLuint GetShadowMapFBO() const noexcept;
		GLuint GetShadowMapWidth() const noexcept;
		GLuint GetShadowMapHeight() const noexcept;
		glm::mat4 GetShadowMapDepthVP() const noexcept;
		GLuint GetShadowMapDepthTexture() const noexcept;
		CubeMapCameraPose* GetShadowMapCameraPose() noexcept;
		GLuint GetShadowCubeMap() const noexcept;

		void SetTransformationMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept;
		void SetShadowMapSize(uint32_t Height, uint32_t Width);

		bool InitShadowMapRender() noexcept;

		// Lighting
	public:

		void CreateLightInfo(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type, eInteractionType interactive);
		glm::vec3 GetLightPos() const noexcept;
		void SetLightPos(const glm::vec3 &pos) noexcept;
		void SetLightType(eLightType type) noexcept;
		eLightType GetLightType() const noexcept;

	private:

		std::shared_ptr<Render> CurrentUsedRender;

		std::unordered_map<std::string, std::shared_ptr<Render>> m_Renders;

		GLuint ShadowMapFBO, DepthTexture;
		GLuint DepthMapWidth, DepthMapHeight;
		glm::mat4 DepthProjectionMatrix, DepthViewMatrix, DepthVP;

		// for point light source
		GLuint ShadowCubeMap;

		CubeMapCameraPose CubeMapIterator[6];

		std::shared_ptr<BaseLighting> pLightInfo;
	};
}

#endif  // CXC_RENDERERMANAGER_H
