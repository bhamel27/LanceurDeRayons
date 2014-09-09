#include <main.h>
#include <log.h>

#pragma warning(push, 0)
#include <time.h>
#include <iostream>

#ifdef _MSC_VER
#include <Windows.h>
#undef ERROR
#endif
#pragma warning(pop)

Log::filestream Log::File;

void Log::SetFile(const char* filename)
{
	File.open(filename);
	if (File.bad())
		exit(-1);
}

Log::stream& Log::Get(Log::Level level)
{
	char tbuf[80];
	os << "- " << GetTime(tbuf);
	os << " " << GetLevelString(level) << ": ";
	os << str(2*(1 + level), ' ');
	messageLevel = level;
	return os;
}

Log::~Log()
{
	os << std::endl;

	str s = os.str();

#if defined(_MSC_VER) && defined(_DEBUG)
	OutputDebugString(s.c_str());
#endif

	std::cerr << s.c_str();
	std::cerr.flush();

	if (File.good())
	{
		File << s.c_str();
		File.flush();
	}

	if (messageLevel == Log::CRITICAL)
	{
#ifdef _DEBUG
		throw std::exception();
#else
		exit(-1);
#endif
	}
}

Log::ccstr Log::GetTime(char buffer[80])
{
	time_t rawtime;
	time(&rawtime);

#ifdef _MSC_VER
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);
#else
	struct tm * timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
#endif

	return buffer;
}

Log::str Log::GetLevelString(Log::Level level)
{
	switch (level)
	{
	case DEBUG:
		return "DEBUG";
	case INFO:
		return "INFO";
	case WARNING:
		return "WARNING";
	case ERROR:
		return "ERROR";
	case CRITICAL:
		return "CRITICAL";
	default:
		return "UNKNOWN";
	}
}
