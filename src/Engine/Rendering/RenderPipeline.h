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
	class LightSource;

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

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);

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

#endif // CXC_PIPELINE_H