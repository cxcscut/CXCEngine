#include "Core/EngineTypes.h"
#include "Geometry/Mesh.h"
#include "Utilities/Singleton.inl"

#ifndef CXC_INPUTS_H
#define CXC_INPUTS_H

namespace cxc {

	enum class eKeyStatus : uint16_t
	{
		PRESSED = 0,
		RELEASED = 1
	};

	using eMouseButtonStatus = eKeyStatus;

	class CXC_ENGINECORE_API InputManager final : public Singleton<InputManager>
	{

	public:
		friend Singleton<InputManager>;

		explicit InputManager();
		~InputManager();

		InputManager(const InputManager &) = delete;
		InputManager(const InputManager &&) = delete;
		InputManager& operator= (const InputManager &) = delete;
		InputManager& operator= (const InputManager &&) = delete;

	public:

		void HandleInput();
		void ProcessingMouseInput();
		void ProcessingKeyboardInput();

	public:

		void InitializeInput(GLFWwindow *window) const noexcept;

	public:

		void GetCurrentCorsorPos(double &PosX, double& PosY);
		eKeyStatus GetKeyStatus(int key);
		void AddKeyPressedStatus(int key);
		void RemoveKeyPressedStatus(int key);

	public:

		// Whether captures the cursor delta position
		bool bCaptureCrusor;

		// Last position of the cursor
		double LastCursorPosX, LastCursorPosY;

	private:

		// Key status
		std::unordered_set<int> KeyStatusMap;
	};

}
#endif // CXC_INPUTS_H
