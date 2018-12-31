#include "World/LogicFramework.h"
#include "Controller/InputManager.h"
#include "Systems/CMovementCtrlSystem.h"
#include "World/World.h"

namespace cxc
{
	LogicFramework::LogicFramework() :
		LogicFrameRates(30)
	{
		MovementCtrlSystem = NewObject<CMovementCtrlSystem>();
	}

	LogicFramework::~LogicFramework()
	{

	}

	void LogicFramework::Tick(float DeltaSeconds)
	{

	}
}
