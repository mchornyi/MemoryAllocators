#pragma once
#include "AllocatorInterface.h"
#include <cassert>
#include <vector>
#include <algorithm>
namespace MemAlloc
{
	class FreeListAllocator : public AllocatorInterface
	{
		struct alignas(sizeof(std::size_t)) AllocationHeader
		{
			std::size_t blockSize;
		};

		const std::size_t cAllocationHeaderSize = sizeof(AllocationHeader);

	public:
		enum PlacementPolicy
		{
			FIND_FIRST,
			FIND_BEST
		};

		FreeListAllocator(FreeListAllocator& freeListAllocator) = delete;

		FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy)
			: AllocatorInterface(totalSize)
		{
			m_pPolicy = pPolicy;
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

			m_start_ptr = malloc(m_totalSize);

			m_freeNodes.reserve(1000);

			Reset();
		}

		struct alignas(sizeof(std::size_t)) MemBlock
		{
			void* memBlock = nullptr;
			size_t blockSize = 0;
		};

		using TMemBlocks = std::vector<MemBlock>;

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			// Search through the free list for a free block that has enough space to allocate our data

			const std::size_t padding = alignment - (cAllocationHeaderSize + size) % alignment;
			const std::size_t requiredSize = cAllocationHeaderSize + size + padding;
			
			SpinlockGuard guard(m_spinlock);
			
			const auto it = Find(requiredSize);
			assert(it != m_freeNodes.end() && "Not enough memory");
			if (it == m_freeNodes.end())
			{
				return nullptr;
			}

			const std::size_t restSize = it->blockSize - requiredSize;
			void* ptr = it->memBlock;

			if (restSize > 0)
			{
				MemBlock newMemNode{reinterpret_cast<void*>(PTR_TO_INT(it->memBlock) + requiredSize), restSize};
				auto newIt = m_freeNodes.emplace(it, newMemNode);
				m_freeNodes.erase(++newIt);
			}
			else
			{
				m_freeNodes.erase(it);
			}

			// Setup data block
			const std::size_t headerAddress = PTR_TO_INT(ptr);
			AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(ptr);
			allocationHeader->blockSize = requiredSize;

			const std::size_t dataAddress = headerAddress + cAllocationHeaderSize;

			m_used += requiredSize;

			assert(dataAddress % alignment == 0 && "Data address must be aligment");

			return (void*)dataAddress;
		}

		bool Free(void* ptr) override
		{
			// Insert it in a sorted position by the address number

			const std::size_t currentAddress = PTR_TO_INT(ptr);
			const std::size_t headerAddress = currentAddress - cAllocationHeaderSize;

			const AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(headerAddress);

			const MemBlock freeMemNode{reinterpret_cast<void*>(headerAddress), 0};
			
			SpinlockGuard guard(m_spinlock);

			const auto it = std::lower_bound(m_freeNodes.begin(), m_freeNodes.end(), freeMemNode,
			                                 [](const MemBlock& memNode, const MemBlock& freeMemNode) {
				                                 return memNode.memBlock < freeMemNode.memBlock;
			                                 });

			const auto newFreeNodeIt = m_freeNodes.insert(it, freeMemNode);

			newFreeNodeIt->blockSize = allocationHeader->blockSize;
			m_used -= newFreeNodeIt->blockSize;

			// Merge contiguous nodes
			MergeMemBlocks(newFreeNodeIt);

			return true;
		}

		void Reset()
		{
			m_used = 0;
			m_freeNodes.clear();

			MemBlock memNode{m_start_ptr, m_totalSize};
			m_freeNodes.emplace_back(memNode);
		}

	private:
		void MergeMemBlocks(TMemBlocks::iterator newFreeNodeIt)
		{
			if (newFreeNodeIt != m_freeNodes.begin() && newFreeNodeIt != m_freeNodes.end())
			{
				const auto freeNodeNext = newFreeNodeIt + 1;
				if (PTR_TO_INT(newFreeNodeIt->memBlock) + newFreeNodeIt->blockSize ==
					PTR_TO_INT(freeNodeNext->memBlock))
				{
					newFreeNodeIt->blockSize += freeNodeNext->blockSize;
					newFreeNodeIt = --m_freeNodes.erase(freeNodeNext);
				}
			}

			if (newFreeNodeIt != m_freeNodes.begin() && newFreeNodeIt != m_freeNodes.end())
			{
				const auto freeNodePrev = newFreeNodeIt - 1;
				if (PTR_TO_INT(freeNodePrev->memBlock) + freeNodePrev->blockSize == PTR_TO_INT(newFreeNodeIt->memBlock))
				{
					freeNodePrev->blockSize += newFreeNodeIt->blockSize;
					m_freeNodes.erase(newFreeNodeIt);
				}
			}
		}

		TMemBlocks::const_iterator Find(const std::size_t size) const
		{
			switch (m_pPolicy)
			{
			case FIND_FIRST:
				return FindFirst(size);
			case FIND_BEST:
				return FindBest(size);
			}

			return m_freeNodes.end();
		}

		TMemBlocks::const_iterator FindFirst(const std::size_t size) const
		{
			return std::find_if(m_freeNodes.begin(), m_freeNodes.end(), [size](const MemBlock& memNode) {
				return memNode.blockSize >= size;
			});
		}

		TMemBlocks::const_iterator FindBest(const std::size_t size) const
		{
			const std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
			auto bestIt = m_freeNodes.begin();

			for (auto it = m_freeNodes.begin(); it != m_freeNodes.end(); ++it)
			{
				if (it->blockSize >= size && (it->blockSize - size < smallestDiff))
				{
					bestIt = it;
				}
			}

			return bestIt;
		}

	private:
		void* m_start_ptr = nullptr;
		PlacementPolicy m_pPolicy;
		TMemBlocks m_freeNodes;
		Spinlock m_spinlock;
	};
}
