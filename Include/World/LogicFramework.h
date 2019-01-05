#include "Utilities/Singleton.inl"

#ifndef CXC_LOGICFRAMEWORK_H
#define CXC_LOGICFRAMEWORK_H

#include <functional>
#include <unordered_map>

namespace cxc
{
	class CMovementCtrlSystem;
	class CActor;

	/* The LogicFramework is the base class that manipulates all the logic systems */
	class CXC_LOGIC_API LogicFramework : public Singleton<LogicFramework>
	{
		friend class Singleton<LogicFramework>;

	public:

		LogicFramework();
		~LogicFramework();

	public:

		void AddActor(std::shared_ptr<CActor> Actor);
		void RemoveActor(std::shared_ptr<CActor> Actor);

		std::shared_ptr<CActor> GetActor(const std::string& ActorName);

	public:

		std::shared_ptr<CMovementCtrlSystem> GetMovementCtrlSystem() { return MovementCtrlSystem; }

	public:

		void SetLogicFrameRates(uint32_t NewFrameRates) { LogicFrameRates = NewFrameRates; }
		uint32_t GetLogicFrameRates() const { return LogicFrameRates; }

	public:

		void Tick(float DeltaSeconds);

	protected:

		// Movement control system
		std::shared_ptr<CMovementCtrlSystem> MovementCtrlSystem;

		// Logic frame rates
		uint32_t LogicFrameRates;

		// Actors that in the logic world
		std::unordered_map<std::string, std::shared_ptr<CActor>> LogicActors;
	};
}

#endif // CXC_LOGICFRAMEWORK_H