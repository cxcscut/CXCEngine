#include "LogicFramework.h"

#ifdef WIN32

#include "..\Controller\InputManager.h"
#include "..\World\World.h"

#else

#include "../Controller/InputManager.h"
#include "../World/World.h"

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
		
	}
}