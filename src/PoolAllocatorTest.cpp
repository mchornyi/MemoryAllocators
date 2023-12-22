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
		PoolAllocator(static_cast<uint32_t>(64) * sMaxChunksNum, 64),
		PoolAllocator(static_cast<uint32_t>(128) * sMaxChunksNum, 128),
		PoolAllocator(static_cast<uint32_t>(256) * sMaxChunksNum, 256),
		PoolAllocator(static_cast<uint32_t>(512) * sMaxChunksNum, 512),
		PoolAllocator(static_cast<uint32_t>(1024) * sMaxChunksNum, 1024),
		PoolAllocator(static_cast<uint32_t>(2048) * sMaxChunksNum, 2048),
		PoolAllocator(static_cast<uint32_t>(3072) * sMaxChunksNum, 3072),
		PoolAllocator(static_cast<uint32_t>(4096) * sMaxChunksNum, 4096),
		PoolAllocator(static_cast<uint32_t>(5120) * sMaxChunksNum, 5120)
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

	Test::GetTestResults().emplace("PoolAllocator    ", duration);
}

TEST_REGISTER(PoolAllocator, RunTest);
