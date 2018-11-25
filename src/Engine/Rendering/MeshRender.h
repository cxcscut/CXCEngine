#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Scene\Lighting.h"

#else

#include "../General/DefineTypes.h"
#include "../Scene/Lighting.h"

#endif

#ifndef CXC_RENDER_H
#define CXC_RENDER_H

#include "RenderPipeline.h"

namespace cxc
{
	class Mesh;
	class Object3D;

	class MeshRender : public std::enable_shared_from_this<MeshRender>
	{

	public:

		MeshRender();
		MeshRender(const std::string& Name);

		virtual ~MeshRender();

	public:

		virtual bool InitializeRender();
		void AddRenderingPipeline(std::shared_ptr<RenderPipeline> pPipeline);

	public:

		std::shared_ptr<RenderPipeline> GetCurrentUsedPipeline() { return CurrentUsedPipeline; }
		std::string GetRenderName() const { return RenderName; }
		void SetRenderName(const std::string& NewName) { RenderName = NewName; }
		std::shared_ptr<RenderPipeline> GetPipelinePtr(const std::string& PipelineName);
		void UsePipeline(std::shared_ptr<RenderPipeline> Pipeline);
		void BindCameraUniforms(GLuint ProgramID);

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);

	protected:

		std::shared_ptr<RenderPipeline> CurrentUsedPipeline;

		// Name of the render
		std::string RenderName;

		// Rendering pipeline
		std::unordered_map<std::string, std::shared_ptr<RenderPipeline>> pRenderPipelines;
	};

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

		const CubeMapCameraPose* GetCubeMapPose() const{ return CubeMapIterator; }
		GLuint GetShadowMapFBO() const noexcept;
		GLuint GetShadowMapWidth() const noexcept;
		GLuint GetShadowMapHeight() const noexcept;
		glm::mat4 GetShadowMapDepthVP() const noexcept;
		GLuint GetShadowMapDepthTexture() const noexcept;
		GLuint GetShadowCubeMap() const noexcept;

	public:

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

#endif // CXC_RENDER_H