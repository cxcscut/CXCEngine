#include "RendererManager.h"

namespace cxc {

	RendererManager::RendererManager()
	{
	}

	RendererManager::~RendererManager()
	{

	}

	void RendererManager::releaseResources() noexcept
	{
		for (auto shader : m_Shaders)
			glDeleteProgram(shader.second.ProgramID);
	}

	GLint RendererManager::GetShaderProgramID(const std::string &name) const noexcept
	{
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
			return it->second.ProgramID;
		else
			return -1;
	}

	GLboolean RendererManager::isShaderLoaded(const std::string &name) const noexcept
	{
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
			return it->second.ProgramID != 0;
		else
			return GL_FALSE;
	}

	GLboolean RendererManager::BindShaderWithExistingProgram(const std::string &name,
															const std::string &vertex_path,
															const std::string &fragment_path) noexcept
	{
		ActiveShader(name);

		return GL_TRUE;
	}

	void RendererManager::SetProgramID(const std::string &name,GLint _ProgramID) noexcept
	{
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
			it->second.ProgramID = _ProgramID;
	}

	GLboolean RendererManager::CheckCompilationStatus(GLuint ShaderID) const noexcept
	{
		GLint ret;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &ret);

		return static_cast<GLboolean>(ret);
	}

	GLboolean RendererManager::CheckLinkageStatus(GLuint ProgramID) const noexcept
	{
		GLint ret;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &ret);

		return static_cast<GLboolean>(ret);
	}

	GLboolean RendererManager::LoadVertexShader(const std::string &vertex_file_path, std::string &vertex_shader_code) const
	{
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

		if (VertexShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(VertexShaderStream, Line))
				VertexShaderCode += "\n" + Line;
			VertexShaderStream.close();
		}
		else
			return GL_FALSE;

		vertex_shader_code = std::move(VertexShaderCode);

		return GL_TRUE;
	}

	GLboolean RendererManager::LoadFragmentShader(const std::string &fragment_file_path, std::string &fragment_shader_code) const
	{
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

		if (FragmentShaderStream.is_open())
		{
			std::string line = "";
			while (getline(FragmentShaderStream, line))
				FragmentShaderCode += "\n" + line;
			FragmentShaderStream.close();
		}
		else
		{
			std::cout << "can not open fragment shader file\n" << std::endl;
			return GL_FALSE;
		}

		fragment_shader_code = std::move(FragmentShaderCode);

		return GL_TRUE;
	}

	GLboolean RendererManager::CompileShader(GLuint ShaderID, const std::string &SourceCode) const
	{
		const char *VertexSourcePointer = SourceCode.c_str();
		glShaderSource(ShaderID, 1, &VertexSourcePointer, nullptr);
		glCompileShader(ShaderID);

		if (CheckCompilationStatus(ShaderID) != GL_TRUE)
			return GL_FALSE;
		else
			return GL_TRUE;
	}

	GLboolean RendererManager::LinkVertexAndFragmentShader(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID) const
	{
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		if (CheckLinkageStatus(ProgramID) != GL_TRUE)
			return GL_FALSE;
		else
			return GL_TRUE;
	}

	void RendererManager::AddProgram(const std::string &name,ProgramStruct &program) noexcept
	{
		if (!program.ProgramID) return;

		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
			it->second = program;
		else
			m_Shaders.insert(std::make_pair(name,program));
	}

	void RendererManager::ActiveShader(const std::string &name) noexcept
	{
		auto ID = GetShaderProgramID(name);
		if (ID > 0) 
			ActiveProgram = ID;
	}

	GLboolean RendererManager::CreateShaderProgram(ProgramStruct &ret,
												const std::string &vertex_file_path,
												const std::string &fragment_file_path)
	{
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint ProgramID = glCreateProgram();

		std::string VertexShaderCode, FragmentShaderCode;

		if (LoadVertexShader(vertex_file_path, VertexShaderCode) == GL_FALSE) {
			std::cout << "VS load failed" << std::endl;
			return GL_FALSE;
		}

		if (LoadFragmentShader(fragment_file_path, FragmentShaderCode) == GL_FALSE) {
			std::cout << "FS load failed" << std::endl;
			return GL_FALSE;
		}

		if (CompileShader(VertexShaderID, VertexShaderCode) != GL_TRUE) {
			std::cout << "VS compile failed" <<std::endl;
			return GL_FALSE;
		}

		if (CompileShader(FragmentShaderID, FragmentShaderCode) != GL_TRUE) {
			std::cout << "FS compile failed" << std::endl;
			return GL_FALSE;
		}

		if (LinkVertexAndFragmentShader(ProgramID, VertexShaderID, FragmentShaderID) != GL_TRUE)
			return GL_FALSE;

		ret.ReConstruct(ProgramID,VertexShaderID,FragmentShaderID);
		
		return GL_TRUE;
	}

	void RendererManager::SetShaderStruct(const std::string &name, ProgramStruct &shader_struct) noexcept
	{
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
			it->second = shader_struct;
	}

}