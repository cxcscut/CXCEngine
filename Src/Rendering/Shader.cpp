#include "Rendering/Shader.h"
#include "Rendering/MeshRenderer.h"

namespace cxc
{
	Shader::Shader():
		bIsCompiled(false)
	{

	}

	Shader::Shader(const std::string& Name, const std::string& FileName, eShaderType Type):
		Shader()
	{
		ShaderType = Type;
		ShaderName = Name;
		ShaderFileName = FileName;
	}

	Shader::~Shader()
	{

	}

	void Shader::AttachToRenderPipeline(std::shared_ptr<MeshRenderPipeline> pRenderPipeline)
	{
		pRenderPipeline->AttachShader(shared_from_this());
	}

	bool Shader::CompileShader()
	{
		if (ShaderType == eShaderType::VERTEX_SHADER)
		{
			ShaderID = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (ShaderType == eShaderType::FRAGMENT_SHADER)
		{
			ShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else
		{
			std::cerr << "Invalid shader type of the shader : " << ShaderName << std::endl;
			return false;
		}

		std::string ShaderSourceCode;
		std::string OutResultLog;

		// Get source code of the  shader
		bool bSuccessful = true;
		bSuccessful &= ReadShaderSourceCode(ShaderFileName, ShaderSourceCode);
		if (!bSuccessful)
		{
			std::cout << "Shader load failed, Path : " << ShaderFileName << std::endl;
			return false;
		}

		// Compile the shader
		bSuccessful &= CompileShader(ShaderID, ShaderSourceCode, OutResultLog);
		if (!bSuccessful)
		{
			std::cout << "Shader compile failed, Path : " << ShaderFileName << std::endl;
			std::cout << OutResultLog << std::endl;
			return false;
		}

		bIsCompiled = true;

		return true;
	}

	bool Shader::ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode)
	{
		if (!FileHelper::FileIsExists(ShaderPath))
			return false;

		SourceCode.clear();
		std::ifstream ShaderStream(ShaderPath, std::ios::in);
		if (ShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(ShaderStream, Line))
			{
				SourceCode += "\n" + Line;
			}
			ShaderStream.close();
		}
		else
			return false;

		return true;
	}

	bool Shader::CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog)
	{
		const char* SourceCodeStr = SourceCode.c_str();
		glShaderSource(ShaderID, 1, &SourceCodeStr, nullptr);
		glCompileShader(ShaderID);

		if (CheckCompilingStatus(ShaderID) != true)
		{
			char szLog[1024] = { 0 };
			GLsizei Loglen = 0;
			glGetShaderInfoLog(ShaderID, 1024, &Loglen, szLog);
			OutResultLog = szLog;

			return false;
		}
		else
			return true;
	}

	bool Shader::CheckCompilingStatus(GLuint ShaderID) const
	{
		GLint Result;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

		return Result > 0 ? true : false;
	}

}