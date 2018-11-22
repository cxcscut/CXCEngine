#include "Pipeline.h"

namespace cxc
{
	RenderingPipeline::RenderingPipeline() :
		VertexShaderID(-1), FragmentShaderID(-1),
		ProgramID(-1), Type(PipelineType::SceneRenderingPipeline)
	{
		pOwnerRender.reset();
	}

	RenderingPipeline::RenderingPipeline(PipelineType NewType)
		: RenderingPipeline()
	{
		Type = NewType;
	}

	RenderingPipeline::~RenderingPipeline()
	{
		if (VertexShaderID)
		{
			if(ProgramID)
				glDetachShader(ProgramID, VertexShaderID);

			glDeleteShader(VertexShaderID);
		}

		if (FragmentShaderID)
		{
			if (ProgramID)
				glDetachShader(ProgramID, FragmentShaderID);

			glDeleteShader(FragmentShaderID);
		}

		if (ProgramID)
			glDeleteProgram(ProgramID);

		pOwnerRender.reset();
	}

	void RenderingPipeline::UsePipeline()
	{
		if (ProgramID > 0)
		{
			glUseProgram(ProgramID);
		}
	}

	bool RenderingPipeline::InitializePipeline()
	{
		VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		ProgramID = glCreateProgram();

		std::string VertexShaderSourceCode, FragmentShaderSourceCode;
		std::string OutResultLog;

		// Get source code of the vertex shader
		bool bSuccessful = true;
		bSuccessful &= ReadShaderSourceCode(VertexShaderPath, VertexShaderSourceCode);
		if (!bSuccessful)
		{
			std::cout << "VS load failed, Path : " << VertexShaderPath << std::endl;
			return false;
		}

		// Get source code of the fragment shader
		bSuccessful &= ReadShaderSourceCode(FragmentShaderPath, FragmentShaderSourceCode);
		if (!bSuccessful)
		{
			std::cout << "FS load failed, Path : " << FragmentShaderPath << std::endl;
			return false;
		}

		// Compile the vertex shader
		bSuccessful &= CompileShader(VertexShaderID, VertexShaderSourceCode, OutResultLog);
		if (!bSuccessful)
		{
			std::cout << "VS compile failed, Path : " << VertexShaderPath << std::endl;
			std::cout << OutResultLog << std::endl;
			return false;
		}

		// Compile the fragment shader
		bSuccessful &= CompileShader(FragmentShaderID, FragmentShaderSourceCode, OutResultLog);
		if (!bSuccessful)
		{
			std::cout << "FS compile failed, Path : " << FragmentShaderPath << std::endl;
			std::cout << OutResultLog << std::endl;
			return false;
		}

		// Link the shaders
		LinkShaders();
		bSuccessful &= CheckLinkingStatus(ProgramID);
		if (!bSuccessful)
		{
			std::cout << "Program linking failed" << FragmentShaderPath << std::endl;
			return false;
		}

		return true;
	}

	bool RenderingPipeline::ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode)
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

	bool RenderingPipeline::CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog)
	{
		const char* SourceCodeStr = SourceCode.c_str();
		glShaderSource(ShaderID, 1, &SourceCodeStr, nullptr);
		glCompileShader(ShaderID);

		if (CheckCompilingStatus(ShaderID) != true)
		{
			char szLog[1024] = {0};
			GLsizei Loglen = 0;
			glGetShaderInfoLog(ShaderID, 1024, &Loglen, szLog);
			OutResultLog = szLog;

			return false;
		}
		else
			return true;
	}

	bool RenderingPipeline::CheckCompilingStatus(GLuint ShaderID) const
	{
		GLint Result;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

		return Result > 0 ? true : false;
	}

	bool RenderingPipeline::CheckLinkingStatus(GLuint ProgramID) const
	{
		GLint Result;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

		return Result > 0 ? true : false;
	}

	void RenderingPipeline::LinkShaders()
	{
		// Attach vertex shader
		glAttachShader(ProgramID, VertexShaderID);

		// Attach fragment shader
		glAttachShader(ProgramID, FragmentShaderID);

		// Link program
		glLinkProgram(ProgramID);
	}
}