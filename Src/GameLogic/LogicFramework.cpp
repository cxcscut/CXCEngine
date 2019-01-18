#include "GameLogic/LogicFramework.h"
#include "Controller/InputManager.h"
#include "Systems/CMovementCtrlSystem.h"
#include "World/World.h"
#include "Utilities/DebugLogger.h"

namespace cxc
{
	LogicFramework::LogicFramework() :
		LogicFrameRates(30), AllocatedGUIDNum(0)
	{
		MovementCtrlSystem = NewObject<CMovementCtrlSystem>();
	}

	LogicFramework::~LogicFramework()
	{

	}

	void LogicFramework::IntializeLogicWorld()
	{
		for (auto ActorPair : LogicActors)
		{
			if (ActorPair.second)
				ActorPair.second->Initialize();
		}
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

	void LogicFramework::RemoveActor(const std::string& ActorName)
	{
		auto ActorIter = LogicActors.find(ActorName);
		if (ActorIter != LogicActors.end())
		{

#ifdef _DEBUG
			std::string DebugString = "Actor(Name = ";
			DebugString += ActorName; DebugString += ", GUID = ";
			DebugString += std::to_string(ActorIter->second->GetObjectGUID()) + ") has been removed \n";

			DEBUG_LOG(eLogType::Verbose, DebugString);
#endif

			LogicActors.erase(ActorIter);
		}
	}

	void LogicFramework::RemoveActor(uint32_t GUID)
	{
		auto RemovingActor = GetActor(GUID);
		if (RemovingActor)
		{
			RemoveActor(RemovingActor->GetName());
		}
	}

	void LogicFramework::RemoveActor(std::shared_ptr<CActor> Actor)
	{
		RemoveActor(Actor->GetName());
	}

	std::shared_ptr<CActor> LogicFramework::GetActor(uint32_t GUID)
	{
		for (auto ActorIter = LogicActors.begin(); ActorIter != LogicActors.end(); ++ActorIter)
		{
			if (ActorIter->second->GetObjectGUID() == GUID)
				return ActorIter->second;
		}

		return nullptr;
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

#ifdef _DEBUG
		std::string DebugString = "Actor(Name = ";
		DebugString += Actor->GetName(); DebugString += ", GUID = ";
		DebugString += std::to_string(Actor->GetObjectGUID()) + ") has been added\n";

		DEBUG_LOG(eLogType::Verbose, DebugString);
#endif

	}
}
