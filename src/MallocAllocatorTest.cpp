#include "Test.h"
#include "benchmark/benchmark.h"

#include <chrono>
#include <iostream>
#include <vector>

using namespace MemAlloc;

static void RunTest()
{
	std::cout << "StartTest: MallocAllocator\n";
	std::cout << "Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	std::vector<void*> memPointers;
	memPointers.reserve(sMaxChunksNum);

	const auto start = std::chrono::high_resolution_clock::now();

	for (std::size_t i = 0; i < sMaxChunksNum; ++i)
	{
		const auto size = rand() % sMaxChunkSize + 1;
		auto* p = malloc(size);
		memPointers.emplace_back(p);
	}

	for (int i = sMaxChunksNum - 1; i >= 0; --i)
	{
		const auto idx = (i != 0 ? rand() % i : 0);
		free(memPointers[idx]);
		memPointers.erase(memPointers.begin() + idx);
	}

	const auto finish = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

	std::cout << "Time = " << duration << "ns\n";

	std::cout << green << "Test Passed!\n" << white;

	Test::GetTestResults().emplace("MallocAllocator   ", duration);
}

TEST_REGISTER(MallocAllocatorTest, RunTest);

static void BM_MallocAlloc(benchmark::State& state)
{
	for (auto _ : state)
	{
		void* p = malloc(1);
		free(p);
		benchmark::DoNotOptimize(p);
	}

	state.SetBytesProcessed(state.iterations());
}

BENCHMARK(BM_MallocAlloc);