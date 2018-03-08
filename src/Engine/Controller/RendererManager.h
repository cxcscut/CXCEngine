#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\inl\Singleton.inl"

#else

#include "../General/DefineTypes.h"
#include "../inl/Singleton.inl"

#endif // WIN32

#ifndef CXC_RENDERERMANAGER_H
#define CXC_RENDERERMANAGER_H

namespace cxc {

	class BaseRender
	{

	public:
		BaseRender(const std::string &vertex_file_path,const std::string &fragment_file_path);
		virtual ~BaseRender();

	public:

		GLuint GetProgramID() { return ProgramID; };

		bool CreateShaderProgram();

		void ActiveShader();

	protected:

		bool LoadVertexShader(const std::string &vertex_file_path, std::string &vertex_shader_code) const;
		bool LoadFragmentShader(const std::string &fragment_file_path, std::string &fragment_shader_code) const;
		bool CompileShader(GLuint ShaderID, const std::string &SourceCode) const;
		bool CheckCompilationStatus(GLuint ShaderID) const noexcept;
		bool CheckLinkageStatus(GLuint ProgramID) const noexcept;
		bool LinkVertexAndFragmentShader(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID) const;

	protected:

		GLuint ProgramID;
		std::string VertexShaderPath, FragmentShaderPath;
		GLuint VertexShader, FragmentShader;

	};

	class ShadowMapRender : public BaseRender
	{
	public:

		ShadowMapRender(GLuint WindowWidth, GLuint WindowHeight, const glm::vec3 &lightPos,
			const std::string &vertex_file_path, const std::string &fragment_file_path);
		~ShadowMapRender();

		bool InitShadowMapRender() noexcept;

		GLuint GetFBO() const noexcept;
		GLuint GetWidth() const noexcept;
		GLuint GetHeight() const noexcept;
		glm::mat4 GetDepthVP() const noexcept;
		glm::vec3 GetLightPos() const noexcept;
		void SetLightPos(const glm::vec3 &pos) noexcept;
		GLuint GetDepthTexture() const noexcept;

	private:

		GLuint m_FBO, depthTexture;
		GLuint WindowWidth, WindowHeight;

		glm::vec3 LightPosition,LightPos;
		glm::mat4 depthProjectionMatrix, depthViewMatrix,depthVP;
	};

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

		GLint GetRenderProgramID(const std::string &name) const noexcept;
		void addRender(const std::string &name,BaseRender * pRender) noexcept;
		void deleteRender(const std::string &name) noexcept;
		void ActivateRender(const std::string &name) noexcept;
		GLuint GetCurrentActiveProgramID() const noexcept ;
		BaseRender* GetRenderPtr(const std::string &name) noexcept;

	// Private date
	private:

		std::unordered_map<std::string,BaseRender*> m_Renders;
		BaseRender *CurrentActiveRender;
	};
}

#endif  // CXC_RENDERERMANAGER_H
