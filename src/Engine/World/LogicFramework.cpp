#include "LogicFramework.h"

#ifdef WIN32

#include "..\Controller\InputManager.h"

#else

#include "../Controller/InputManager.h"

#endif

namespace cxc
{
	LogicFramework::LogicFramework():
		LogicFrameRates(30)
	{

	}

	LogicFramework::~LogicFramework()
	{

	}

	void LogicFramework::LogicTick(float DeltaSeconds)
	{
		// Tick the InputManager
		InputManager::GetInstance()->Tick(DeltaSeconds);
	}
}