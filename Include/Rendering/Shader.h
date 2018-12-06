#include "General/DefineTypes.h"
#include "Utilities/FileHelper.h"

#ifndef CXC_SHADER_H
#define CXC_SHADER_H

namespace cxc
{
	class MeshRenderPipeline;

	enum class eShaderType : uint32_t
	{
		VERTEX_SHADER = 0,
		FRAGMENT_SHADER = 1,
		GEOMETRY_SHADER = 2,
		SHADER_TYPESIZE = 3
	};

	class Shader : public std::enable_shared_from_this<Shader>
	{

		friend class RenderManager;

	public:
		Shader();
		Shader(const std::string& ShaderName, const std::string& ShaderFileName, eShaderType Type);
		virtual ~Shader();

	public:

		bool ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode);
		bool CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog);
		bool CheckCompilingStatus(GLuint ShaderID) const;

	public:

		eShaderType GetShaderType() const { return ShaderType; }
		GLuint GetShaderID() const { return ShaderID; }
		bool IsCompiled() const { return bIsCompiled; }
		const std::string& GetShaderFileName() const { return ShaderFileName; }
		const std::string& GetShaderName() const { return ShaderName; }

	public:

		bool CompileShader();
		void AttachToRenderPipeline(std::shared_ptr<MeshRenderPipeline> pRenderPipeline);

	protected:

		// Whether the shader is compiled
		bool bIsCompiled;

		// Name of the shader
		std::string ShaderName;
		
		// Type of the shader
		eShaderType ShaderType;

		// ID of the shader
		GLuint ShaderID;

		// File path of the shader
		std::string ShaderFileName;
	};
}

#endif // CXC_SHADER_H