#ifndef CXC_LOGICTHREAD_H
#define CXC_LOGICTHREAD_H

#include "Core/EngineCore.h"
#include <thread>

namespace cxc
{
	class LogicFramework;
	class World;

	/* CGameLogicThread is the base class that manipulates the thread of game logic */
	class CXC_LOGIC_API CGameLogicThread
	{
	public :
		CGameLogicThread();
		virtual ~CGameLogicThread();

	public:

		void SetLogicFramework(std::shared_ptr<LogicFramework> Framework) { pLogicFramework = Framework; }
		void SetOwnerWorld(std::shared_ptr<World> pWorld) { pOwnerWorld = pWorld; }


	public:
		
		virtual void LogicThreadFunc();
		virtual void BeginThread();
		virtual void ReleaseThread();

	private:

		// Weak pointer back to the world
		std::weak_ptr<World> pOwnerWorld;

		// Pointer to the LogicFramework
		std::shared_ptr<LogicFramework> pLogicFramework;

		// Thread
		std::thread GameLogicThread;

		// Last tick time of the logic thread
		float LastTickSeconds;
	};
}

#endif // CXC_LOGICTHREAD_H