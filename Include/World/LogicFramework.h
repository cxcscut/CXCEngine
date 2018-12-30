#include "Utilities/Singleton.inl"

#ifndef CXC_LOGICFRAMEWORK_H
#define CXC_LOGICFRAMEWORK_H

#include <functional>

namespace cxc
{
	class CMovementCtrlSystem;

	/* The LogicFramework is the base class that manipulates all the logic systems */
	class CXC_LOGIC_API LogicFramework : public Singleton<LogicFramework>
	{
		friend class Singleton<LogicFramework>;

	public:

		LogicFramework();
		~LogicFramework();

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
	};
}

#endif // CXC_LOGICFRAMEWORK_H