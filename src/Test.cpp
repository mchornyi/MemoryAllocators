#include "Test.h"
#include <iostream>

using namespace MemAlloc;

const std::size_t MemAlloc::sMaxChunksNum = 1000;
const std::size_t MemAlloc::sMaxChunkSize = 5120;

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

	std::cout << "\n";
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
