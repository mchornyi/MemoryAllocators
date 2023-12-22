#include "FreeListAllocator.h"
#include "Test.h"
#include <vector>
using namespace MemAlloc;

static void RunTest()
{
	std::cout << "StartTest: FreeListAllocator\n";
	std::cout << "Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	FreeListAllocator allocator(sMaxChunksNum * sMaxChunkSize, MemAlloc::FreeListAllocator::FIND_FIRST);
	allocator.Init();

	std::vector<void*> memPointers;
	memPointers.reserve(sMaxChunksNum);

	const auto start = std::chrono::high_resolution_clock::now();

	for (std::size_t i = 0; i < sMaxChunksNum; ++i)
	{
		const auto size = rand() % sMaxChunkSize + 1;
		auto* p = allocator.Allocate(size);
		memPointers.emplace_back(p);
	}

	for (int i = sMaxChunksNum - 1; i >= 0; --i)
	{
		const auto idx = (i != 0 ? rand() % i : 0);
		allocator.Free(memPointers[idx]);
		memPointers.erase(memPointers.begin() + idx);
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

	Test::GetTestResults().emplace("FreeListAllocator ", duration);
}

TEST_REGISTER(FreeListAllocatorTest, RunTest);
