#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Utilities\FileHelper.h"

#else

#include "../General/DefineTypes.h"
#include "../Utilities/FileHelper.h"

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

		~RenderPipeline();

	public:

		void SetVertexShaderPath(const std::string& Path) { VertexShaderPath = Path; }
		void SetFragmentShaderPath(const std::string& Path) { FragmentShaderPath = Path; }
		void SetOwnerRender(std::shared_ptr<MeshRender> pNewOwner) { pOwnerRender = pNewOwner; }
		void SetPipelineName(const std::string& Name) { PipelineName = Name; }

		std::string GetPipelineName() const { return PipelineName; }
		GLint GetVertexShaderID() const { return VertexShaderID; }
		GLint GetFragmentShaderID() const { return FragmentShaderID; }
		GLint GetPipelineProgramID() const { return ProgramID; }
		std::string GetVertexShaderPath() const { return VertexShaderPath; }
		std::string GetFragmentShaderPath() const { return FragmentShaderPath; }

	public:

		void BindLightUniforms(std::shared_ptr<BaseLighting> pLight);

	public:

		bool InitializePipeline();
		void UsePipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh);
		virtual void Render(std::shared_ptr<Mesh> pMesh);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh);

	private:

		bool ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode);
		bool CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog);
		bool CheckCompilingStatus(GLuint ShaderID) const;
		bool CheckLinkingStatus(GLuint ProgramID) const;
		void LinkShaders();

	protected:

		// Weak pointer to the render that owns it
		std::weak_ptr<MeshRender> pOwnerRender;

	protected:

		std::string PipelineName;

		// GLSL of the shader
		std::string VertexShaderPath, FragmentShaderPath;

		// ID of the vertex shader
		GLint VertexShaderID;

		// ID of the fragment shader
		GLint FragmentShaderID;

		// ID of the program
		GLint ProgramID;
	};

	class ShadowedMeshRenderPipeline : public RenderPipeline
	{
	public:

		ShadowedMeshRenderPipeline();

		~ShadowedMeshRenderPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh) override;
	};

	class ShadowMapCookingPipeline : public RenderPipeline
	{
	public:

		ShadowMapCookingPipeline();
		~ShadowMapCookingPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh) override;

	public:

		void CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh);
		void RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh);

	};
}

#endif // CXC_PIPELINE_H