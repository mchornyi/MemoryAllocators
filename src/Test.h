#pragma once
#include <set>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
inline std::ostream& white(std::ostream& s)
{
#ifdef _WIN32
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
	                        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
	return s;
}

inline std::ostream& red(std::ostream& s)
{
#ifdef _WIN32
	const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
	                        FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
	return s;
}

inline std::ostream& green(std::ostream& s)
{
#ifdef _WIN32
	const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
	                        FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
	return s;
}

namespace MemAlloc
{
	const std::size_t sMaxChunksNum = 1000;
	const std::size_t sMaxChunkSize = 5120;
	using RunTestFunc = void (*)();

	struct custom_compare final
	{
		bool operator()(const std::pair<std::string, std::size_t>& left,
		                const std::pair<std::string, std::size_t>& right) const
		{
			return left.second < right.second;
		}
	};

	using TestResultsType = std::set<std::pair<std::string, std::size_t>, custom_compare>;

	struct Test
	{
		static void Run();
		static void Register(RunTestFunc func);
		static TestResultsType& GetTestResults();
	};

	struct TestRegistration
	{
		TestRegistration(RunTestFunc func)
		{
			Test::Register(func);
		}
	};

#define TEST_REGISTER(name, func) \
namespace name { \
TestRegistration testRegistration(func); \
};
} // namespace MemAlloc
