#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Utilities\FileHelper.h"
#include "..\Rendering\Shader.h"

#else

#include "../General/DefineTypes.h"
#include "../Utilities/FileHelper.h"
#include "../Rendering/Shader.h"

#endif

#ifndef CXC_PIPELINE_H
#define CXC_PIPELINE_H

namespace cxc
{
	class MeshRender;
	class Mesh;
	class Object3D;
	class BaseLighting;

	class RenderPipeline
	{
	public:

		RenderPipeline();
		RenderPipeline(const std::string& Name);

		virtual ~RenderPipeline();

	public:

		void SetOwnerRender(std::shared_ptr<MeshRender> pNewOwner) { pOwnerRender = pNewOwner; }
		void SetPipelineName(const std::string& Name) { PipelineName = Name; }

		GLuint GetPipelineProgramID() const { return ProgramID; }
		std::string GetPipelineName() const { return PipelineName; }
		std::shared_ptr<Shader> GetCurrentAttachedShader(eShaderType ShaderType);

	public:

		void BindLightUniforms(std::shared_ptr<BaseLighting> pLight);

	public:

		bool InitializePipeline();
		void UsePipeline();

	public:

		bool CheckLinkingStatus(std::string& OutResultLog) const;
		void AttachShader(std::shared_ptr<Shader> pShader);
		void DetachShader(eShaderType AttachmentLoc);
		bool LinkShaders();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);

	protected:

		// Weak pointer to the render that owns it
		std::weak_ptr<MeshRender> pOwnerRender;

	protected:

		// Name of the pipeline
		std::string PipelineName;

		// Shaders attached to the pipeline
		std::vector<std::shared_ptr<Shader>> pShaders;

		// ID of the program
		GLint ProgramID;
	};

	class ShadowedMeshRenderPipeline : public RenderPipeline
	{
	public:

		ShadowedMeshRenderPipeline();

		virtual ~ShadowedMeshRenderPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
	};

	class ShadowMapCookingPipeline : public RenderPipeline
	{
	public:

		ShadowMapCookingPipeline();
		virtual ~ShadowMapCookingPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;

	public:

		void CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		void RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);

	};
}

#endif // CXC_PIPELINE_H