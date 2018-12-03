#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Scene\Lighting.h"

#else

#include "../General/DefineTypes.h"
#include "../Scene/Lighting.h"

#endif

#ifndef CXC_RENDER_H
#define CXC_RENDER_H

#include "Shader.h"


namespace cxc
{
	class Mesh;
	class Object3D;
	class LightSource;
	class MeshRenderPipeline;

	/* Base class for all render */
	class MeshRender : public std::enable_shared_from_this<MeshRender>
	{

	public:

		MeshRender();
		MeshRender(const std::string& Name);

		virtual ~MeshRender();

	public:

		virtual bool InitializeRender() = 0;

	public:

		std::shared_ptr<MeshRenderPipeline> GetCurrentUsedPipeline() { return CurrentUsedPipeline; }
		std::string GetRenderName() const { return RenderName; }
		void SetRenderName(const std::string& NewName) { RenderName = NewName; }
		void UsePipeline(std::shared_ptr<MeshRenderPipeline> Pipeline);
		void BindCameraUniforms(GLuint ProgramID);

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};

	protected:

		std::shared_ptr<MeshRenderPipeline> CurrentUsedPipeline;

		// Name of the render
		std::string RenderName;
	};

	/* Base class for all the render pipeline */
	class MeshRenderPipeline
	{
	public:

		MeshRenderPipeline();
		MeshRenderPipeline(const std::string& Name);

		virtual ~MeshRenderPipeline();

	public:

		void SetOwnerRender(std::shared_ptr<MeshRender> pNewOwner) { pOwnerRender = pNewOwner; }
		void SetPipelineName(const std::string& Name) { PipelineName = Name; }

		GLuint GetPipelineProgramID() const { return ProgramID; }
		std::string GetPipelineName() const { return PipelineName; }
		std::shared_ptr<Shader> GetAttachedShader(const std::string& ShaderName);

	public:

		void BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights);

	public:

		bool InitializePipeline();

	public:

		bool CheckLinkingStatus(std::string& OutResultLog) const;
		void AttachShader(std::shared_ptr<Shader> pShader);
		void DetachShader(std::shared_ptr<Shader> pShader);
		bool LinkShaders();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) {};

	protected:

		// Weak pointer to the render that owns it
		std::weak_ptr<MeshRender> pOwnerRender;

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