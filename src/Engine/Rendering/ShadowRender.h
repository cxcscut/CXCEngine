#include "MeshRender.h"

#ifndef CXC_SHADOWRENDER_H
#define CXC_SHADOWRENDER_H

namespace cxc
{
	class Mesh;
	class BaseLighting;

	class ShadowMapRender : public MeshRender
	{
		using CubeMapCameraPose = struct CubeMapCameraPose {
			GLenum CubeMapFace;
			glm::vec3 Direction;
			glm::vec3 UpVector;
		};

	public:

		ShadowMapRender();
		ShadowMapRender(const std::string& RenderName);

		virtual ~ShadowMapRender();

	public:

		const CubeMapCameraPose* GetCubeMapPose() const { return CubeMapIterator; }
		GLuint GetShadowMapFBO() const noexcept;
		GLuint GetShadowMapWidth() const noexcept;
		GLuint GetShadowMapHeight() const noexcept;
		glm::mat4 GetShadowMapDepthVP() const noexcept;
		GLuint GetShadowMapDepthTexture() const noexcept;
		GLuint GetShadowCubeMap() const noexcept;

	public:

		void SetShadowMapResolution(GLuint Width, GLuint Height);
		void SetLightSpaceMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept;
		void SetShadowMapTextureSize(uint32_t Height, uint32_t Width);

		bool InitShadowMapRender(const std::vector<std::shared_ptr<BaseLighting>>& Lights) noexcept;

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;

	private:

		bool bIsShadowTextureCreate;

		// FBO
		GLuint FrameBufferObjectID;

		// Depth texture
		GLuint DepthMapTexture;

		// Height and Width of the depth texture
		GLuint DepthMapHeight, DepthMapWidth;

		// Matrix of the shadow map rendring
		glm::mat4 DepthProjectionMatrix, DepthViewMatrix, DepthVP;

		// Cubemap for omni-directional light
		GLuint ShadowCubeMap;

		// CubeMap iterator
		CubeMapCameraPose CubeMapIterator[6];

	};
}

#endif // CXC_SHADOWRENDER_H