#include "GameLogic/LogicThread.h"
#include "World/World.h"

namespace cxc
{
	CGameLogicThread::CGameLogicThread():
		pLogicFramework(nullptr)
	{
		pOwnerWorld.reset();
	}

	CGameLogicThread::~CGameLogicThread()
	{
		pOwnerWorld.reset();
	}


	void CGameLogicThread::LogicThreadFunc()
	{
		if (pLogicFramework != nullptr && !pOwnerWorld.expired() && pOwnerWorld.lock() != nullptr)
		{
			auto pWorld = pOwnerWorld.lock();
			while (!pWorld->GameOver)
			{
				auto CurrentWorldSeconds = pWorld->GetWorldSeconds();
				auto SecondsBetweenFrames = CurrentWorldSeconds - LastTickSeconds;
				auto FixedLogicDeltaSeconds = 1 / static_cast<float>(pLogicFramework->GetLogicFrameRates());

				if (SecondsBetweenFrames >= FixedLogicDeltaSeconds)
				{
					auto TickTimes = static_cast<uint32_t>(SecondsBetweenFrames / FixedLogicDeltaSeconds);

					pLogicFramework->Tick(TickTimes * FixedLogicDeltaSeconds);
				}
				else
				{
					auto SleepMilliSeconds = static_cast<int>((FixedLogicDeltaSeconds - SecondsBetweenFrames) * 1000);

					// Waiting
					std::this_thread::sleep_for(std::chrono::milliseconds(SleepMilliSeconds));

					pLogicFramework->Tick(FixedLogicDeltaSeconds);

					CurrentWorldSeconds = pWorld->GetWorldSeconds();
				}

				LastTickSeconds = CurrentWorldSeconds;
			}
		}
	}

	void CGameLogicThread::BeginThread()
	{
		// Bind the threading func
		std::function<void(void)> ThreadFunc;
		ThreadFunc = std::bind(&CGameLogicThread::LogicThreadFunc, this);

		// Create thread
		GameLogicThread = std::thread(ThreadFunc);
	}

	void CGameLogicThread::ReleaseThread()
	{
		if (GameLogicThread.joinable())
			GameLogicThread.join();
	}
}