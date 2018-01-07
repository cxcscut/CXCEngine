#include "..\General\DefineTypes.h"

#ifndef CXC_WINDOW_H
#define CXC_WINDOW_H

#include "..\inl\Singleton.inl"

namespace cxc {

	class WindowManager final : public Singleton<WindowManager>
	{

	public:
		friend Singleton<WindowManager>;

		explicit WindowManager();
		~WindowManager();

		WindowManager(const WindowManager &) = delete;
		WindowManager(const WindowManager &&) = delete;

		WindowManager& operator=(const WindowManager &) = delete;
		WindowManager& operator=(const WindowManager &&) = delete;

	public:

		GLboolean _CreateWindow();
		GLboolean PrepareResourcesForCreating() const noexcept;
		void InitContext() const noexcept;

		void SetWindowHeight(GLint height) noexcept;
		void SetWindowWidth(GLint width) noexcept;
		void SetSamplingLevel(GLint level) noexcept;
		void SetDriverVersion(GLint High,GLint Low) noexcept;
		void SetForwardCompatibleFlag(GLboolean compatible) noexcept;
		void SetWindowTitle(const std::string &title) noexcept;
		void SetEnableDepthFlag(GLboolean depthtest) noexcept;
		void SetBackGroundColor(float r,float g,float b,float alpha) noexcept;
		void SetWindowPos(GLint x,GLint y) noexcept;
		bool WindowIsReady() const noexcept;

		const Color &GetBackGroundColor() const noexcept;
		GLint GetWindowHeight() const noexcept;
		GLint GetWindowWidth() const noexcept;
		GLint GetSamplingLevel() const noexcept;
		GLint GetVersionHighByte() const noexcept;
		GLint GetVersionLowByte() const noexcept;
		GLboolean isSupportForwardCompatible() const noexcept;
		GLboolean isEnableDepthTest() const noexcept;
		const std::string &GetWindowTitle() const noexcept;
		GLFWwindow *GetWindowHandle() const noexcept;

		bool InitGL() const noexcept;
		bool isDecoraded;
		GLint x_pos, y_pos;

	private:

		GLint m_WindowHeight, m_WindowWidth;
		GLint m_SamplingLevel;
		GLint LowByteVersion, HighByteVersion;
		GLboolean isForwardCompatible;
		GLboolean isEnableDepth;
		std::string m_WindowTitle;
		GLFWwindow* m_WindowHandle;
		Color m_BackGroundColor;

		bool isReady;
	};
}

#endif // CXC_WINDOW_H
