#pragma once
#include "AllocatorInterface.h"
#include <cassert>
#include <array>

namespace MemAlloc
{
	enum MergePolicy
	{
		ENABLE_FAST_MERGE = 1,
		ENABLE_FULL_MERGE = 2,
		ENABLE_FULL_MERGE_IF_NO_SPACE = 3
	};
	const MergePolicy cMergePolicy(ENABLE_FULL_MERGE_IF_NO_SPACE);

	const ThreadPolicy cFreeListThreadPolicy(NONE);

	constexpr std::size_t L1Size = 32768; // 32KiB
	class alignas(sizeof(std::size_t)) FreeListAllocator : public AllocatorInterface
	{
		struct alignas(sizeof(std::size_t)) AllocationHeader
		{
			std::size_t blockSize;
		};

		static const std::size_t cAllocationHeaderSize = sizeof(AllocationHeader);

	public:
		FreeListAllocator(FreeListAllocator& freeListAllocator) = delete;

		FreeListAllocator(const std::size_t totalSize)
			: AllocatorInterface(totalSize)
		{
		}

		~FreeListAllocator() override
		{
			free(m_start_ptr);
			m_start_ptr = nullptr;
		}

		void Init() override
		{
			if (m_start_ptr != nullptr)
			{
				free(m_start_ptr);
				m_start_ptr = nullptr;
			}

			m_start_ptr = static_cast<char*>(malloc(m_totalSize));

			Reset();
		}

		struct alignas(sizeof(std::size_t)) MemBlock
		{
			std::size_t memBlockOffset = 0;
			std::size_t blockSize = 0;
		};

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			// Search through the free list for a free block that has enough space to allocate our data

			const std::size_t padding = alignment - (cAllocationHeaderSize + size) % alignment;
			const std::size_t requiredSize = cAllocationHeaderSize + static_cast<uint32_t>(size) + padding;

			switch (cFreeListThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.lock();
				break;
			case NONE:
				break;
			}

			const int freeMemBlockIndex = FindFreeMemBlockIndex(requiredSize);
			assert(freeMemBlockIndex != -1 && "Not enough memory");
			if (freeMemBlockIndex == -1)
			{
				return nullptr;
			}

			const std::size_t restSize = m_freeMemBlocks[freeMemBlockIndex].blockSize - requiredSize;
			void* freeMemBlock = m_start_ptr + m_freeMemBlocks[freeMemBlockIndex].memBlockOffset;

			if (restSize > 0)
			{
				m_freeMemBlocks[freeMemBlockIndex] = {
					(m_freeMemBlocks[freeMemBlockIndex].memBlockOffset +
						static_cast<uint32_t>(requiredSize)),
					static_cast<uint32_t>(restSize)
				};
			}
			else
			{
				std::swap(m_freeMemBlocks[freeMemBlockIndex - 1], m_freeMemBlocks[m_currSize]);
				--m_currSize;
			}

			// Setup data block
			AllocationHeader* allocationHeader = static_cast<AllocationHeader*>(freeMemBlock);

			void* resultPtr = (PTR_TO_CHAR(allocationHeader) + cAllocationHeaderSize);
			assert(PTR_TO_INT(resultPtr) % alignment == 0 && "Data address must be aligment");

			m_used += requiredSize;

			allocationHeader->blockSize = requiredSize;

			switch (cFreeListThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.unlock();
				break;
			case NONE:
				break;
			}

			return resultPtr;
		}

		bool Free(void* ptr) override
		{
			// Insert it in a sorted position by the address number

			AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(PTR_TO_CHAR(ptr) -
				cAllocationHeaderSize);

			const MemBlock freeMemBlock{
				static_cast<uint32_t>(reinterpret_cast<char*>(allocationHeader) - m_start_ptr), 0
			};

			switch (cFreeListThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.lock();
				break;
			case NONE:
				break;
			}

			assert(!ShouldFullMerge() && "There is no free space for free a memory block!");

			m_freeMemBlocks[m_currSize] = freeMemBlock;
			++m_currSize;

			m_freeMemBlocks[m_currSize - 1].blockSize = allocationHeader->blockSize;
			m_used -= m_freeMemBlocks[m_currSize - 1].blockSize;

			// Merge contiguous memBlocks

			switch (cMergePolicy)
			{
			case ENABLE_FAST_MERGE:
				FastMergeMemBlocks(static_cast<int>(m_currSize) - 1);
				break;
			case ENABLE_FULL_MERGE:
				FullMergeMemBlocks();
				break;
			case ENABLE_FULL_MERGE_IF_NO_SPACE:
				if (ShouldFullMerge())
				{
					FullMergeMemBlocks();
				}
				break;
			}

			switch (cFreeListThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.unlock();
				break;
			case NONE:
				break;
			}

			return true;
		}

		void Reset()
		{
			m_used = 0;
			m_freeMemBlocks[0] = {0, static_cast<uint32_t>(m_totalSize)};
			m_currSize = 1;
		}

		void FullMergeMemBlocks()
		{
			for (int i = static_cast<int>(m_currSize) - 1; i >= 0; --i)
			{
				FastMergeMemBlocks(i);
			}
		}

		bool IsFullyMerged() const
		{
			return m_currSize == 1 && m_freeMemBlocks[0].memBlockOffset == 0 && m_freeMemBlocks[0].blockSize ==
				m_totalSize;
		}

	private:
		bool ShouldFullMerge() const
		{
			return m_currSize == m_freeMemBlocks.size();
		}

		void FastMergeMemBlocks(const int pivotIndex)
		{
			const auto& pivotMemBlock = m_freeMemBlocks[pivotIndex];

			for (int i = pivotIndex - 1; i >= 0; --i)
			{
				if (m_freeMemBlocks[i].memBlockOffset + m_freeMemBlocks[i].blockSize == pivotMemBlock.memBlockOffset)
				{
					m_freeMemBlocks[i].blockSize += pivotMemBlock.blockSize;
					std::swap(m_freeMemBlocks[pivotIndex], m_freeMemBlocks[m_currSize - 1]);
					--m_currSize;
					return;
				}

				if (m_freeMemBlocks[i].memBlockOffset == pivotMemBlock.memBlockOffset + pivotMemBlock.blockSize)
				{
					m_freeMemBlocks[i].memBlockOffset = pivotMemBlock.memBlockOffset;
					m_freeMemBlocks[i].blockSize += pivotMemBlock.blockSize;
					std::swap(m_freeMemBlocks[pivotIndex], m_freeMemBlocks[m_currSize - 1]);
					--m_currSize;
					return;
				}
			}
		}

		int FindFreeMemBlockIndex(const std::size_t size) const
		{
			if (m_currSize == 0 || ShouldFullMerge() || m_used == m_totalSize)
			{
				return -1;
			}

			std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
			int bestIndex = -1;

			for (int i = static_cast<int>(m_currSize) - 1; i >= 0; --i)
			{
				if (m_freeMemBlocks[i].blockSize >= size && (static_cast<int>(m_freeMemBlocks[i].blockSize) - size <
					smallestDiff))
				{
					smallestDiff = m_freeMemBlocks[i].blockSize - size;
					bestIndex = i;
				}
			}

			return bestIndex;
		}

	private:
		char* m_start_ptr = nullptr;
		std::size_t m_currSize = 0;
		Spinlock m_spinlock;
		// We must fit to 32 KiB = L1 cache size
		std::array<MemBlock, (L1Size - sizeof(AllocatorInterface) - sizeof(m_start_ptr) - sizeof(m_currSize) - sizeof(
			           m_spinlock)) / sizeof(MemBlock)> m_freeMemBlocks;
	};
}
