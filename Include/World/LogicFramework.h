#include "Utilities/Singleton.inl"

#ifndef CXC_LOGICFRAMEWORK_H
#define CXC_LOGICFRAMEWORK_H

#include <functional>

namespace cxc
{
	class LogicFramework final : public Singleton<LogicFramework>
	{
		friend class Singleton<LogicFramework>;

	public:

		LogicFramework();
		~LogicFramework();

	public:

		void SetLogicFrameRates(uint32_t NewFrameRates) { LogicFrameRates = NewFrameRates; }
		uint32_t GetLogicFrameRates() const { return LogicFrameRates; }
		void SetLogicEntry(std::function<void(void)> NewLogicEntry) { LogicEntryFunction = NewLogicEntry; }

	public:

		void LogicTick(float DeltaSeconds);

	private:

		// Game logic entry
		std::function<void(void)> LogicEntryFunction;

		// Logic frame rates
		uint32_t LogicFrameRates;
	};
}

#endif // CXC_LOGICFRAMEWORK_H