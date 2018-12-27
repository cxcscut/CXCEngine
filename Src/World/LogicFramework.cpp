#include "World/LogicFramework.h"
#include "Controller/InputManager.h"
#include "Systems/CMovementCtrlSystem.h"
#include "World/World.h"

namespace cxc
{
	LogicFramework::LogicFramework():
		LogicFrameRates(30)
	{
		LogicEntryFunction = []() {};

		MovementCtrlSystem = NewObject<CMovementCtrlSystem>();
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