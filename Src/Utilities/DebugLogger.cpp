#include "Utilities/DebugLogger.h"
#include <iostream>
#include <chrono>

#ifdef WIN32

#include "windows.h"

#endif

namespace cxc
{
	DebugLogger::DebugLogger() :
		bEnableWriteLogFile(false), DebugLogFileDirectory(""),
		Verbosity(true)
	{
		
	}

	DebugLogger::~DebugLogger()
	{
		if (LogFile.is_open())
			LogFile.close();
	}

	std::string DebugLogger::GetSystemDate() const
	{
		auto tt = std::chrono::system_clock::to_time_t
		(std::chrono::system_clock::now());
		struct tm* ptm = localtime(&tt);
		char date[60] = { 0 };
		sprintf(date, "%d-%02d-%02d",
			(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday);
		return std::string(date);
	}

	std::string DebugLogger::GetSystemTime() const
	{
		auto tt = std::chrono::system_clock::to_time_t
		(std::chrono::system_clock::now());
		struct tm* ptm = localtime(&tt);
		char date[60] = { 0 };
		sprintf(date, "%d-%02d-%02d,%02d:%02d:%02d",
			(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
			(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
		return std::string(date);
	}

	void DebugLogger::DebugLog(eLogType Type, const std::string& LogMsg)
	{
#ifdef _DEBUG

		std::string OutPutLogMsg;
		std::string CurrentSystemTime = GetSystemTime();
		switch (Type)
		{
		case cxc::eLogType::Error:
#ifdef WIN32

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			OutPutLogMsg = CurrentSystemTime + "; Error: " + LogMsg;

#endif
			std::cerr << OutPutLogMsg << std::endl;
			break;
		case cxc::eLogType::Warning:
#ifdef WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
			OutPutLogMsg = CurrentSystemTime + "; Warning: " + LogMsg;

#endif
			std::clog << OutPutLogMsg << std::endl;
			break;
		case cxc::eLogType::Verbose:
			if (!Verbosity)
				return;

			OutPutLogMsg = CurrentSystemTime + "; Verbose: " + LogMsg;
			std::cout << OutPutLogMsg << std::endl;
			break;
		default:
			break;
		}

		WriteLogFile(OutPutLogMsg);

#endif
	}

	void DebugLogger::WriteLogFile(const std::string LogMsg)
	{
		if (bEnableWriteLogFile && !LogMsg.empty())
		{
			// Write the log into the log file
			std::string LogFileFullPath = DebugLogFileDirectory + GetSystemDate() + ".txt";
			if (!LogFile.is_open())
			{
				LogFile.open(LogFileFullPath, std::ios::app);
				LogFile << std::endl;
				LogFile << "-----------------------------------------------------------------" << std::endl;
				LogFile << std::endl;
			}

			LogFile << LogMsg << std::endl;	
		}
	}
}