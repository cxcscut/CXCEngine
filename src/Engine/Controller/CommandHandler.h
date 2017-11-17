#include "..\General\DefineTypes.h"
#include "..\Graphics\SceneManager.h"

class Robothand;

#ifndef CXC_COMMANDHANDLER
#define CXC_COMMANDHANDLER

namespace cxc
{
	class CommandHandler final
	{

	public:
		
		CommandHandler();
		~CommandHandler();

	public:

		static void ProcessingCommandStr();
		static void MatchCommand(const std::string &command);

	};
}

#endif // CXC_COMMANDHANDLER
