#include "LinearAllocator.h"
#include "Test.h"

#include <array>
#include <chrono>
#include <iostream>

using namespace MemAlloc;

static void RunTest()
{
	std::cout << "StartTest: LinerAllocator\n";
	std::cout << "Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	LinearAllocator allocator(sMaxChunksNum * sMaxChunkSize);

	const auto start = std::chrono::high_resolution_clock::now();

	for (std::size_t i = 0; i < sMaxChunksNum; ++i)
	{
		const auto size = rand() % sMaxChunkSize + 1;
		allocator.Allocate(size);
	}

	allocator.Reset();

	const auto finish = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

	std::cout << "Time = " << duration << "ns\n";

	if (allocator.GetUsedSize() > 0)
	{
		std::cout << red << "Test Failed!\n" << white;
	}
	else
	{
		std::cout << green << "Test Passed!\n" << white;
	}

	Test::GetTestResults().emplace("LinerAllocator   ", duration);
}

TEST_REGISTER(LinerAllocator, RunTest);
