#include "SubMeshRenderer.h"
#include "ShadowRenderPipeline.h"

#ifndef CXC_SHADOWRENDER_H
#define CXC_SHADOWRENDER_H

namespace cxc
{
	class SubMesh;
	class LightSource;

	/* ShadowRenderer does not support casting shadows of multiple dynamic lights */
	class CXC_ENGINECORE_API ShadowRenderer : public SubMeshRenderer
	{
		using CubeMapCameraPose = struct CubeMapCameraPose {
			GLenum CubeMapFace;
			glm::vec3 Direction;
			glm::vec3 UpVector;
		};

	public:

		ShadowRenderer();
		ShadowRenderer(const std::string& RendererName);

		virtual ~ShadowRenderer();

	public:

		const CubeMapCameraPose* GetCubeMapPose() const { return CubeMapIterator; }
		GLuint GetShadowMapFBO() const noexcept;
		GLuint GetShadowMapSize() const noexcept;
		glm::mat4 GetShadowMapDepthVP() const noexcept;
		GLuint GetShadowMapDepthTexture() const noexcept;
		GLuint GetShadowCubeMap() const noexcept;

	public:

		void SetShadowMapResolution(GLuint Size);
		void SetLightSpaceMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept;

		bool InitShadowMapRender(const std::vector<std::shared_ptr<LightSource>>& Lights) noexcept;
		void ClearShadowMapBuffer(const std::vector<std::shared_ptr<LightSource>>& Lights);

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context) override;

	private:

		// Whether the shadow textures have been created
		bool bIsShadowTextureCreate;

		// Whether the depth textures have been cleared
		bool bHasDepthTexturesCleared;

		// FBO
		GLuint FrameBufferObjectID;

		// Depth texture
		GLuint DepthMapTexture;

		// Height and Width of the depth texture
		GLuint DepthMapSize;

		// Matrix of the shadow map rendring
		glm::mat4 DepthProjectionMatrix, DepthViewMatrix, DepthVP;

		// Cubemap for omni-directional light
		GLuint ShadowCubeMap;

		// CubeMap iterator
		CubeMapCameraPose CubeMapIterator[6];

	};
}

#endif // CXC_SHADOWRENDER_H