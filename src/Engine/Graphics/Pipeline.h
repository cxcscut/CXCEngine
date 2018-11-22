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
	class Render;

	enum class PipelineType : uint16_t
	{
		SceneRenderingPipeline = 0,
		ShadowMapPipeline = 1
	};

	class RenderingPipeline
	{
	public:

		RenderingPipeline();
		RenderingPipeline(PipelineType NewType);

		~RenderingPipeline();

	public:

		void SetVertexShaderPath(const std::string& Path) { VertexShaderPath = Path; }
		void SetFragmentShaderPath(const std::string& Path) { FragmentShaderPath = Path; }
		void SetOwnerRender(std::shared_ptr<Render> pNewOwner) { pOwnerRender = pNewOwner; }
		void SetPipelineType(PipelineType NewType) { Type = NewType; }

		PipelineType GetPipelineType() const { return Type; }
		GLint GetVertexShaderID() const { return VertexShaderID; }
		GLint GetFragmentShaderID() const { return FragmentShaderID; }
		GLint GetPipelineProgramID() const { return ProgramID; }
		std::string GetVertexShaderPath() const { return VertexShaderPath; }
		std::string GetFragmentShaderPath() const { return FragmentShaderPath; }

	public:

		bool InitializePipeline();
		void UsePipeline();

	private:

		bool ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode);
		bool CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog);
		bool CheckCompilingStatus(GLuint ShaderID) const;
		bool CheckLinkingStatus(GLuint ProgramID) const;
		void LinkShaders();

	private:

		PipelineType Type;

		// GLSL of the shader
		std::string VertexShaderPath, FragmentShaderPath;

		// Weak pointer to the render that owns it
		std::weak_ptr<Render> pOwnerRender;

		// ID of the vertex shader
		GLint VertexShaderID;

		// ID of the fragment shader
		GLint FragmentShaderID;

		// ID of the program
		GLint ProgramID;
	};
}

#endif // CXC_PIPELINE_H