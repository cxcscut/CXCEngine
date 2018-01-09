#include "Window.h"

namespace cxc {

	WindowManager::WindowManager()
		: m_WindowHeight(300),
		m_WindowWidth(400),
		m_SamplingLevel(4),
		LowByteVersion(3), HighByteVersion(3),
		isForwardCompatible(GL_TRUE),
		isEnableDepth(GL_TRUE), m_BackGroundColor(),
		isReady(false),isDecoraded(false),
		x_pos(m_WindowWidth/2),y_pos(m_WindowHeight/2)

 	{

	}

	WindowManager::~WindowManager()
	{

	}

	void WindowManager::SetWindowHeight(GLint height) noexcept
	{
		m_WindowHeight = height;
	}

	void WindowManager::SetWindowWidth(GLint width) noexcept
	{
		m_WindowWidth = width;
	}

	void WindowManager::SetSamplingLevel(GLint level) noexcept
	{
		m_SamplingLevel = level;
	}

	void WindowManager::SetDriverVersion(GLint high, GLint low) noexcept
	{
		LowByteVersion = low;
		HighByteVersion = high;
	}

	void WindowManager::SetForwardCompatibleFlag(GLboolean compatible) noexcept
	{
		isForwardCompatible = compatible;
	}

	void WindowManager::SetWindowTitle(const std::string &title) noexcept
	{
		m_WindowTitle = title;
	}

	GLint WindowManager::GetWindowHeight() const noexcept
	{
		return m_WindowHeight;
	}

	GLint WindowManager::GetWindowWidth() const noexcept
	{
		return m_WindowWidth;
	}

	GLint WindowManager::GetSamplingLevel() const noexcept
	{
		return m_SamplingLevel;
	}

	GLint WindowManager::GetVersionHighByte() const noexcept
	{
		return HighByteVersion;
	}

	const Color &WindowManager::GetBackGroundColor() const noexcept
	{
		return m_BackGroundColor;
	}

	GLint WindowManager::GetVersionLowByte() const noexcept
	{
		return LowByteVersion;
	}

	const std::string &WindowManager::GetWindowTitle() const noexcept
	{
		return m_WindowTitle;
	}

	GLFWwindow *WindowManager::GetWindowHandle() const noexcept
	{
		return m_WindowHandle;
	}

	GLboolean WindowManager::isSupportForwardCompatible() const noexcept
	{
		return isForwardCompatible;
	}

	void WindowManager::SetEnableDepthFlag(GLboolean depthtest) noexcept
	{
		isEnableDepth = depthtest;
	}

	void WindowManager::SetBackGroundColor(float r,float g,float b,float alpha) noexcept
	{
		m_BackGroundColor = Color(r,g,b,alpha);
	}

	GLboolean WindowManager::isEnableDepthTest() const noexcept
	{
		return isEnableDepth;
	}

	bool WindowManager::WindowIsReady() const noexcept{
		return isReady;
	}

	GLboolean WindowManager::_CreateWindow()
	{

		m_WindowHandle = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle.c_str(), nullptr, nullptr);

		if (m_WindowHandle == nullptr)
			return GL_FALSE;
		else
		{
			// Set window pos
			SetWindowPos(x_pos,y_pos);
			isReady = true;
			return GL_TRUE;
		}
	}

	void WindowManager::SetWindowPos(GLint x, GLint y) noexcept
	{
		if(m_WindowHandle)
			glfwSetWindowPos(m_WindowHandle, x, y);
	}

	bool WindowManager::InitGL() const noexcept {
		if (glfwInit())
			return true;
		else
			return false;
	}

	GLboolean WindowManager::PrepareResourcesForCreating() const noexcept
	{

        glfwWindowHint(GLFW_SAMPLES, m_SamplingLevel);					                // sampling
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, HighByteVersion);					// high byte of version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LowByteVersion);						// low byte of version
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, HighByteVersion);	                // forwarad compatible

#ifdef WIN32

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#else

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#endif

		glfwWindowHint(GLFW_DECORATED, isDecoraded); // remove caption bar and frame

		return GL_TRUE;
	}

	void WindowManager::InitContext() const noexcept
	{
		glfwMakeContextCurrent(m_WindowHandle);
		glewExperimental = true;
		glClearColor(m_BackGroundColor.Red,
					m_BackGroundColor.Green,
					m_BackGroundColor.Blue,
					m_BackGroundColor.Alpha);

		if (isEnableDepth == GL_TRUE) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		}

		glEnable(GL_CULL_FACE);

	}

}
