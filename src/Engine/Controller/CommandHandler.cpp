#include "CommandHandler.h"
#include "..\EngineFacade\EngineFacade.h"

auto EnginePtr = cxc::EngineFacade::GetInstance();

namespace cxc
{

	CommandHandler::CommandHandler() {}
	
	CommandHandler::~CommandHandler() {}

	void CommandHandler::ProcessingCommandStr()
	{
		std::string input_str;

		while (!EnginePtr->GameOver)
		{
			std::cin >> input_str;

			MatchCommand(input_str);
		}

	}

	void CommandHandler::MatchCommand(const std::string &command)
	{
		
	}
}