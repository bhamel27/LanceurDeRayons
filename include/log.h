#pragma once

#pragma warning(disable : 4127)

#include <sstream>
#include <fstream>

class Log
{
public:
	typedef std::ostringstream stream;
	typedef std::ofstream filestream;
	typedef std::string str;
	typedef char* cstr;
	typedef const char* ccstr;

	static void SetFile(const char* filename);

	enum Level
	{
		DEBUG = 4,
		INFO = 3,
		WARNING = 2,
		ERROR = 1,
		CRITICAL = 0
	};

	Log() {}
	virtual ~Log();

	stream& Get(Level level = DEBUG);
#ifdef _DEBUG
	const static Level ReportingLevel = DEBUG;
#else
	const static Level ReportingLevel = WARNING;
#endif

protected:
	ccstr GetTime(char buffer[80]);
	str GetLevelString(Level level);
	stream os;

	static filestream File;
private:
	Level messageLevel;
};

#define _LOG(level) if(level > Log::ReportingLevel) ; else Log().Get(level)

// Causes program termination: anything beyond this is never executed!
#define _LOG_CRIT() _LOG(Log::CRITICAL)
#define _LOG_ERR() _LOG(Log::ERROR)
#define _LOG_WARN() _LOG(Log::WARNING)
#define _LOG_INFO() _LOG(Log::INFO)
#define _LOG_DBG() _LOG(Log::DEBUG)

#define _ASSERT_FALSE(action, message) if(!(action)) { _LOG_CRIT() << message; throw exception(); }
#define _ASSERT_TRUE(action, message) if((action)) { _LOG_CRIT() << message; throw exception(); }
#define _ASSERT_NZERO(action, message) if((action) != 0) { _LOG_CRIT() << message; throw exception(); }
#define _ASSERT_NULL(action, message) if((action) == nullptr) { _LOG_CRIT() << message; throw exception(); }