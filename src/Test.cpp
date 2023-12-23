#include "Test.h"
#include <iostream>

using namespace MemAlloc;

std::set<RunTestFunc>& GetTests()
{
	static std::set<RunTestFunc> sTests;
	return sTests;
}

TestResultsType& Test::GetTestResults()
{
	static TestResultsType sTestResults;
	return sTestResults;
}

void PrintResults()
{
	std::cout << "Test results:\n";
	for (auto& result : Test::GetTestResults())
	{
		std::cout << result.first << ": " << result.second << " ns\n";
	}
}

void Test::Run()
{
	for (auto& func : GetTests())
	{
		func();
		std::cout << "\n";
	}

	PrintResults();
}

void Test::Register(RunTestFunc func)
{
	GetTests().emplace(func);
}
