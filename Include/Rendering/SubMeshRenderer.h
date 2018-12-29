#include "Core/EngineTypes.h"
#include "Scene/Lighting.h"

#ifndef CXC_RENDER_H
#define CXC_RENDER_H

#include "Shader.h"

namespace cxc
{
	class SubMesh;
	class Mesh;
	class LightSource;
	class SubMeshRenderPipeline;
	class RendererContext;

	/* Base class for all renderer */
	class CXC_ENGINECORE_API SubMeshRenderer : public std::enable_shared_from_this<SubMeshRenderer>
	{

	public:

		SubMeshRenderer();
		SubMeshRenderer(const std::string& Name);

		virtual ~SubMeshRenderer();

	public:

		virtual bool InitializeRenderer() = 0;

	public:

		std::shared_ptr<SubMeshRenderPipeline> GetCurrentUsedPipeline() { return CurrentUsedPipeline; }
		std::string GetRendererName() const { return RendererName; }
		void SetRendererName(const std::string& NewName) { RendererName = NewName; }
		void UsePipeline(std::shared_ptr<SubMeshRenderPipeline> Pipeline);
		void PushPipeline(std::shared_ptr<SubMeshRenderPipeline> Pipeline);
		void PopPipeline();
		void BindCameraUniforms(GLuint ProgramID);

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context,const std::vector<std::shared_ptr<LightSource>>& Lights) = 0;

	protected:

		// Currently active pipeline
		std::shared_ptr<SubMeshRenderPipeline> CurrentUsedPipeline;

		// Rendering queue, which is rendered from the front to back
		std::deque<std::shared_ptr<SubMeshRenderPipeline>> RenderingQueue;

		// Name of the render
		std::string RendererName;
	};

	/* Base class for all the render pipeline */
	class CXC_ENGINECORE_API SubMeshRenderPipeline
	{
	public:

		SubMeshRenderPipeline();
		SubMeshRenderPipeline(const std::string& Name);

		virtual ~SubMeshRenderPipeline();

	public:

		void SetOwnerRenderer(std::shared_ptr<SubMeshRenderer> pNewOwner) { pOwnerRenderer = pNewOwner; }
		void SetPipelineName(const std::string& Name) { PipelineName = Name; }

		GLuint GetPipelineProgramID() const { return ProgramID; }
		std::string GetPipelineName() const { return PipelineName; }
		std::shared_ptr<Shader> GetAttachedShader(const std::string& ShaderName);

	public:

		virtual void BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights, std::vector<GLuint>& SubroutineIndices);

	public:

		bool InitializePipeline();

	public:

		bool CheckLinkingStatus(std::string& OutResultLog) const;
		void AttachShader(std::shared_ptr<Shader> pShader);
		void DetachShader(std::shared_ptr<Shader> pShader);
		bool LinkShaders();

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) {};

	protected:

		// Weak pointer to the render that owns it
		std::weak_ptr<SubMeshRenderer> pOwnerRenderer;

	protected:

		// Name of the pipeline
		std::string PipelineName;

		// Shaders attached to the pipeline
		std::unordered_map<std::string, std::shared_ptr<Shader>> pShaders;

		// ID of the program
		GLint ProgramID;
	};
}

#endif // CXC_RENDER_H