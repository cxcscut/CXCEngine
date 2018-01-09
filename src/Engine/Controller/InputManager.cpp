#include "InputManager.h"

namespace cxc {

	InputManager::InputManager()
		: xpos(0.0f),ypos(0.0f)
	{

	}

	InputManager::InputManager(GLint x,GLint y) 
		:	xpos(static_cast<GLdouble>(x)),
		ypos(static_cast<GLdouble>(y))
	{

	}

	InputManager::~InputManager()
	{

	}

	GLint InputManager::GetXPos() const noexcept
	{
		return xpos;
	}

	GLint InputManager::GetYPos() const noexcept
	{
		return ypos;
	}

	void InputManager::SetMouseScreenPos(GLFWwindow *window, GLdouble x, GLdouble y) const noexcept
	{
		glfwSetCursorPos(window, x, y);
	}

	void InputManager::SetMouseCordinates(GLdouble newxpos, GLdouble newypos) noexcept
	{
		xpos = newxpos;
		ypos = newypos;
	}
	 
	void InputManager::UpdateMousePos(GLFWwindow *window) noexcept
	{
		glfwGetCursorPos(window, &xpos, &ypos);
	}

	void InputManager::SetInputModel(GLFWwindow *window) const noexcept
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}
