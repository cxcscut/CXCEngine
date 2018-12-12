#include "World/LogicFramework.h"
#include "Controller/InputManager.h"
#include "World/World.h"

namespace cxc
{
	LogicFramework::LogicFramework():
		LogicFrameRates(30)
	{
		LogicEntryFunction = []() {};
	}

	LogicFramework::~LogicFramework()
	{

	}

	void LogicFramework::LogicTick(float DeltaSeconds)
	{
		// Tick the game logic entry
		LogicEntryFunction();
	}
}