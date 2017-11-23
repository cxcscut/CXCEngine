#include "..\General\DefineTypes.h"

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

#include "..\inl\Singleton.inl"

namespace cxc {

	class RendererManager final : public Singleton<RendererManager>
	{
	public: 
		friend Singleton<RendererManager>;

		explicit RendererManager();
		~RendererManager();

		RendererManager(const RendererManager &) = delete;
		RendererManager(const RendererManager &&) = delete;

		RendererManager& operator=(const RendererManager &) = delete;
		RendererManager& operator=(const RendererManager &&) = delete;

	public:

		GLboolean CreateShaderProgram(ProgramStruct &ret,
								const std::string &vertex_file_path, 
								const std::string &fragment_file_path);

		GLint GetShaderProgramID(ShaderType Type) const noexcept;
		GLboolean GetVertexShaderArray(ShaderType Type,std::vector<GLint> &VertexArray) const noexcept;
		GLboolean GetFragmentShaderArray(ShaderType Type,std::vector<GLint> &VertexArray) const noexcept;
		GLboolean isShaderLoaded(ShaderType Type) const noexcept;

		GLboolean BindShaderWithExistingProgram(ShaderType Type,
											const std::string &vertex_path,
											const std::string &fragment_path) noexcept;
		
		void SetProgramID(ShaderType Type,GLint ProgramID) noexcept;
		
		void CreateProgram(ShaderType Type,ProgramStruct &program) noexcept;
		void SetShaderStruct(ShaderType type,ProgramStruct &shader_struct) noexcept;

		void releaseResources() noexcept;

	// Private date
	private:

		ProgramStruct SpriteShader, FontShader;

	// Status checking
	private:

		GLboolean LoadVertexShader(const std::string &vertex_file_path, std::string &vertex_shader_code) const;
		GLboolean LoadFragmentShader(const std::string &fragment_file_path, std::string &fragment_shader_code) const;
		GLboolean CompileShader(GLuint ShaderID, const std::string &SourceCode) const;
		GLboolean CheckCompilationStatus(GLuint ShaderID) const noexcept;
		GLboolean CheckLinkageStatus(GLuint ProgramID) const noexcept;
		GLboolean LinkVertexAndFragmentShader(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID) const;
		
	};
}

#endif  // CXC_RENDERERMANAGER_H