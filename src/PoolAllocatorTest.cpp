#include "PoolAllocator.h"
#include "Test.h"

#include <array>
#include <vector>

using namespace MemAlloc;

// Holds several pool allocator of different sizes
class PoolAllocators final
{
public:
	PoolAllocators()
	{
		for (auto& allocator : mAllocators)
		{
			allocator.Init();
		}
	}

	void* Allocate(uint32_t size)
	{
		void* result = nullptr;

		for (auto& allocator : mAllocators)
		{
			if (size <= allocator.GetChunkSize())
			{
				result = allocator.Allocate(size);
				break;
			}
		}

		if (!result)
		{
			std::cout << "Warning[RedNetPhotonAllocator]: All allocators are full! Fallback to system call\n";
			return malloc(size);
		}

		return result;
	}

	void Free(void* p)
	{
		bool found = false;
		for (auto& allocator : mAllocators)
		{
			found = allocator.Free(p);
			if (found)
			{
				break;
			}
		}

		if (!found)
		{
			free(p);
		}
	}

	uint32_t GetTotalSize() const
	{
		uint32_t totalSize = 0;
		for (const auto& allocator : mAllocators)
		{
			totalSize += allocator.GetTotalSize();
		}

		return totalSize;
	}

	uint32_t GetUsedSize() const
	{
		uint32_t usedSize = 0;
		for (const auto& allocator : mAllocators)
		{
			usedSize += allocator.GetUsedSize();
		}

		return usedSize;
	}

private:
	std::array<PoolAllocator, 9> mAllocators = {
		PoolAllocator(sMaxChunksNum, 64),
		PoolAllocator(sMaxChunksNum, 128),
		PoolAllocator(sMaxChunksNum, 256),
		PoolAllocator(sMaxChunksNum, 512),
		PoolAllocator(sMaxChunksNum, 1024),
		PoolAllocator(sMaxChunksNum, 2048),
		PoolAllocator(sMaxChunksNum, 3072),
		PoolAllocator(sMaxChunksNum, 4096),
		PoolAllocator(sMaxChunksNum, 5120)
	};
};

static void RunTest()
{
	std::cout << "StartTest: PoolAllocator\n";
	std::cout << "Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	PoolAllocators allocators;

	std::vector<void*> memPointers;
	memPointers.reserve(sMaxChunksNum);

	const auto start = std::chrono::high_resolution_clock::now();

	for (std::size_t i = 0; i < sMaxChunksNum; ++i)
	{
		const auto size = rand() % sMaxChunkSize + 1;
		auto* p = allocators.Allocate(size);
		memPointers.emplace_back(p);
	}

	for (int i = sMaxChunksNum - 1; i >= 0; --i)
	{
		const auto idx = (i != 0 ? rand() % i : 0);
		allocators.Free(memPointers[idx]);
		memPointers.erase(memPointers.begin() + idx);
	}

	const auto finish = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

	std::cout << "Time = " << duration << "ns\n";

	if (allocators.GetUsedSize() > 0)
	{
		std::cout << red << "Test Failed!\n" << white;
	}
	else
	{
		std::cout << green << "Test Passed!\n" << white;
	}

	Test::GetTestResults().emplace("PoolAllocator     ", duration);
}

TEST_REGISTER(PoolAllocatorTest, RunTest);

static void RunMultiThreadTest()
{
	std::cout << "StartMultiThreadTest: PoolAllocator\n";
	std::cout << "Desc: Creates 9 pool allocators of different chunk size. Create 2 threads. Each thread allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.\n";
	std::cout << "MaxChunksNum " << sMaxChunksNum << "\n";
	std::cout << "MaxChunkSize " << sMaxChunkSize << "\n";

	PoolAllocators allocators;

	auto threadFunc = [&allocators](){
		std::vector<void*> memPointers;
		memPointers.reserve(sMaxChunksNum);

		for (std::size_t i = 0; i < sMaxChunksNum / 2; ++i)
		{
			const auto size = rand() % sMaxChunkSize + 1;
			auto* p = allocators.Allocate(size);
			memPointers.emplace_back(p);
		}

		const std::size_t memPointersNum = memPointers.size();
		for (int i = memPointersNum - 1; i >= 0; --i)
		{
			const auto idx = (i != 0 ? rand() % i : 0);
			allocators.Free(memPointers[idx]);
			memPointers.erase(memPointers.begin() + idx);
		}
	};
	
	const auto start = std::chrono::high_resolution_clock::now();
	
	std::thread thread1(threadFunc);
	std::thread thread2(threadFunc);
	
	thread1.join();
	thread2.join();

	const auto finish = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

	std::cout << "Time = " << duration << "ns\n";

	if (allocators.GetUsedSize() > 0)
	{
		std::cout << red << "Test Failed!\n" << white;
	}
	else
	{
		std::cout << green << "Test Passed!\n" << white;
	}

	Test::GetTestResults().emplace("PoolAlloc2Threads ", duration);
}

TEST_REGISTER(PoolAllocatorMultiTest, RunMultiThreadTest);
