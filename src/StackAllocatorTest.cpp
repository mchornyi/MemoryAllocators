#include "StackAllocator.h"
#include "Test.h"
#include "benchmark/benchmark.h"

#include <list>
#include <stack>

using namespace MemAlloc;

static void RunTest()
{
	std::cout << "StartTest: StackAllocator\n";	
	std::cout << "Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	StackAllocator allocator(sMaxChunksNum * sMaxChunkSize);
	allocator.Init();

	std::list<void*> memPointers;

	const auto start = std::chrono::high_resolution_clock::now();

	for (std::size_t i = 0; i < sMaxChunksNum; ++i)
	{
		const auto size = rand() % sMaxChunkSize + 1;
		auto* p = allocator.Allocate(size);
		memPointers.emplace_back(p);
	}

	while (!memPointers.empty())
	{
		allocator.Free(memPointers.back());
		memPointers.pop_back();
	}

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

	Test::GetTestResults().emplace("StackAllocator    ", duration);
}

TEST_REGISTER(StackAllocatorTest, RunTest);

static void BM_StackAlloc(benchmark::State& state)
{
	StackAllocator allocator(sMaxChunksNum * sMaxChunkSize);
	allocator.Init();

	for (auto _ : state)
	{
		auto* p = allocator.Allocate(1);
		allocator.Free(p);
	}

	state.SetBytesProcessed(state.iterations());
}

BENCHMARK(BM_StackAlloc);