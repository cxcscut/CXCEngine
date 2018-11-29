#include "InputManager.h"

#if WIN32

#include "..\World\World.h"

#else

#include "../World/World.h"

#endif

namespace cxc {

	const float MovingStep = 0.8f;

	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto pInputMgr = InputManager::GetInstance();
		if (action == GLFW_PRESS)
		{
			pInputMgr->AddKeyPressedStatus(key);
		}
		else if (action == GLFW_RELEASE)
		{
			pInputMgr->RemoveKeyPressedStatus(key);
		}
	}

	void MouseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
	{
		auto pInputMgr = InputManager::GetInstance();
		if ((action == GLFW_PRESS) && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			pInputMgr->bCaptureCrusor = true;
		}
		else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			pInputMgr->bCaptureCrusor = false;
		}
	}

	InputManager::InputManager() :
		LastCursorPosX(0.0f), LastCursorPosY(0.0f),
		bCaptureCrusor(false)
	{

	}

	InputManager::~InputManager()
	{

	}

	void InputManager::Tick(float DeltaSeconds)
	{
		// Processing keyboard input
		ProcessingKeyboardInput();

		// Processing mouse input
		ProcessingMouseInput();
	}

	void InputManager::ProcessingMouseInput()
	{
		auto pInputMgr = InputManager::GetInstance();
		if (bCaptureCrusor)
		{
			auto pEngine = World::GetInstance();
			auto pRender = pEngine->pSceneMgr->pRenderMgr->GetCurrentUsedRender();
			if (!pRender)
				return;

			auto CurrentUsedPipeline = pRender->GetCurrentUsedPipeline();
			if (!CurrentUsedPipeline) return;

			auto ProgramID = CurrentUsedPipeline->GetPipelineProgramID();
			auto pCamera = pEngine->pSceneMgr->pCamera;

			double CurrentX, CurrentY;
			double CursorDeltaX, CursorDeltaY;
			pInputMgr->GetCurrentCorsorPos(CurrentX, CurrentY);
			CursorDeltaX = CurrentX - pInputMgr->LastCursorPosX;
			CursorDeltaY = CurrentY - pInputMgr->LastCursorPosY;
			pInputMgr->LastCursorPosX = CurrentX;
			pInputMgr->LastCursorPosY = CurrentY;

			float DeltaHorizontal = -PI * (CursorDeltaX / pEngine->pWindowMgr->GetWindowWidth() / 2);
			float DeltaVertical = PI * (CursorDeltaY / pEngine->pWindowMgr->GetWindowHeight() / 2);

			auto Forward = pCamera->CameraOrigin - pCamera->EyePosition;
			auto RotMatrix = glm::rotate(glm::mat4(1.0f), DeltaHorizontal, pCamera->UpVector);
			RotMatrix = glm::rotate(RotMatrix, DeltaVertical, pCamera->GetCameraRightVector());
			auto RotatedForward = RotMatrix * glm::vec4(Forward, 1);
			pCamera->CameraOrigin = pCamera->EyePosition + (glm::vec3)RotatedForward;

			pCamera->ComputeViewMatrix();
			pCamera->BindCameraUniforms(ProgramID);
		}
		else
		{
			pInputMgr->GetCurrentCorsorPos(pInputMgr->LastCursorPosX, pInputMgr->LastCursorPosY);
		}
	}

	void InputManager::ProcessingKeyboardInput()
	{
		auto pEngine = World::GetInstance();
		auto pCamera = pEngine->pSceneMgr->pCamera;
		auto pRender = pEngine->pSceneMgr->pRenderMgr->GetCurrentUsedRender();
		if (!pRender)
			return;

		auto CurrentUsedPipeline = pRender->GetCurrentUsedPipeline();
		if (!CurrentUsedPipeline) return;

		auto ProgramID = CurrentUsedPipeline->GetPipelineProgramID();

		if (GetKeyStatus(GLFW_KEY_W) == eKeyStatus::PRESSED)
		{
			// Camera moving forward
			auto ForwardVector = pCamera->GetCameraForwardVector();
			auto MovingVector = ForwardVector * MovingStep;
			pCamera->EyePosition += MovingVector;
			pCamera->CameraOrigin += MovingVector;
		}
		if (GetKeyStatus(GLFW_KEY_S) == eKeyStatus::PRESSED)
		{
			// Camera moving backward
			auto ForwardVector = pCamera->GetCameraForwardVector();
			auto MovingVector = ForwardVector * MovingStep;
			pCamera->EyePosition -= MovingVector;
			pCamera->CameraOrigin -= MovingVector;
		}
		if (GetKeyStatus(GLFW_KEY_A) == eKeyStatus::PRESSED)
		{
			// Camera moving left
			auto RightVector = pCamera->GetCameraRightVector();
			auto MovingVector = RightVector * MovingStep;
			pCamera->EyePosition += MovingVector;
			pCamera->CameraOrigin += MovingVector;
		}
		if (GetKeyStatus(GLFW_KEY_D) == eKeyStatus::PRESSED)
		{
			// Camera moving right
			auto RightVector = pCamera->GetCameraRightVector();
			auto MovingVector = RightVector * MovingStep;
			pCamera->EyePosition -= MovingVector;
			pCamera->CameraOrigin -= MovingVector;
		}

		pCamera->ComputeViewMatrix();
		pCamera->BindViewMatrix(ProgramID);
	}

	void InputManager::InitializeInput(GLFWwindow *window) const noexcept
	{
		auto pWindowMgr = WindowManager::GetInstance();

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// Register input callback
		glfwSetKeyCallback(pWindowMgr->GetWindowHandle(), KeyBoradCallBack);
		glfwSetMouseButtonCallback(pWindowMgr->GetWindowHandle(), MouseButtonCallBack);
	}

	void InputManager::GetCurrentCorsorPos(double &PosX, double& PosY)
	{
		auto pWindowMgr = WindowManager::GetInstance();
		glfwGetCursorPos(pWindowMgr->GetWindowHandle(), &PosX, &PosY);
	}

	eKeyStatus InputManager::GetKeyStatus(int key)
	{
		auto KeyIter = KeyStatusMap.find(key);
		if (KeyIter != KeyStatusMap.end())
		{
			return eKeyStatus::PRESSED;
		}
		else
			return eKeyStatus::RELEASED;
	}

	void InputManager::AddKeyPressedStatus(int key)
	{
		KeyStatusMap.insert(key);
	}

	void InputManager::RemoveKeyPressedStatus(int key)
	{
		auto KeyIter = KeyStatusMap.find(key);
		if (KeyIter != KeyStatusMap.end())
		{
			KeyStatusMap.erase(KeyIter);
		}
	}

}
