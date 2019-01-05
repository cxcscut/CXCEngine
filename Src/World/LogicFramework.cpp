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
		// Tick all the logic actor
		for (auto ActorPair : LogicActors)
		{
			if (ActorPair.second)
				ActorPair.second->Tick(DeltaSeconds);
		}
	}

	void LogicFramework::RemoveActor(std::shared_ptr<CActor> Actor)
	{
		auto ActorIter = LogicActors.find(Actor->GetName());
		if (ActorIter != LogicActors.end())
		{
			LogicActors.erase(ActorIter);
		}
	}

	std::shared_ptr<CActor> LogicFramework::GetActor(const std::string& ActorName)
	{
		auto ActorIter = LogicActors.find(ActorName);
		if (ActorIter != LogicActors.end())
			return ActorIter->second;
		else
			return nullptr;
	}

	void LogicFramework::AddActor(std::shared_ptr<CActor> Actor)
	{
		LogicActors.insert(std::make_pair(Actor->GetName(), Actor));
	}
}
