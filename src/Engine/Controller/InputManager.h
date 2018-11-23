#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Scene\Object3D.h"

#include "..\Utilities\Singleton.inl"

#else

#include "../General/DefineTypes.h"
#include "../Scene/Object3D.h"

#include "../Utilities/Singleton.inl"

#endif // WIN32

#ifndef CXC_INPUTS_H
#define CXC_INPUTS_H

namespace cxc {

	class InputManager final : public Singleton<InputManager>
	{

	public:
		friend Singleton<InputManager>;

		explicit InputManager();

		InputManager(GLint x,GLint y);
		~InputManager();

		InputManager(const InputManager &) = delete;
		InputManager(const InputManager &&) = delete;
		InputManager& operator= (const InputManager &) = delete;
		InputManager& operator= (const InputManager &&) = delete;

	public:

		void Tick(float DeltaSeconds);

	public:

		GLint GetXPos() const noexcept;
		GLint GetYPos() const noexcept;

		void SetMouseScreenPos(GLFWwindow *window, GLdouble x, GLdouble y) const noexcept;
		void SetMouseCordinates(GLdouble newxpos, GLdouble newypos) noexcept;
		void SetInputModel(GLFWwindow *window) const noexcept;

	// State update
	public:

		void UpdateMousePos(GLFWwindow *window) noexcept;

	// Private data
	public:

		GLdouble xpos, ypos;
	};

}
#endif // CXC_INPUTS_H
