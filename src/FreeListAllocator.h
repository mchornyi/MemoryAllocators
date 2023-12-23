#pragma once
#include <cassert>
#include "AllocatorInterface.h"

namespace MemAlloc
{
	class FreeListAllocator : public AllocatorInterface
	{
		struct alignas(sizeof(std::size_t)) FreeHeader
		{
			std::size_t blockSize;
		};

		struct alignas(sizeof(std::size_t)) AllocationHeader
		{
			std::size_t blockSize;
			char padding;
		};

		const std::size_t cAllocationHeaderSize = sizeof(AllocationHeader);

		typedef SinglyLinkedList<FreeHeader>::Node FreeNode;

	public:
		enum PlacementPolicy
		{
			FIND_FIRST,
			FIND_BEST
		};

		FreeListAllocator(FreeListAllocator& freeListAllocator) = delete;

		FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy)
			: AllocatorInterface(totalSize), m_freeList()
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

			Reset();
		}

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			// Search through the free list for a free block that has enough space to allocate our data
			FreeNode *freeNode = nullptr, *previousNode = nullptr;

			const std::size_t requiredSize = cAllocationHeaderSize + size;
			Find(requiredSize, previousNode, freeNode);

			assert(freeNode != nullptr && "Not enough memory");

			const std::size_t restSize = freeNode->data.blockSize - requiredSize;

			if (restSize > 0)
			{
				const char padding = CalculatePadding(PTR_TO_INT(freeNode) + requiredSize, alignment);

				// We have to split the block into the data block and a free block of size 'rest'
				FreeNode* newFreeNode = reinterpret_cast<FreeNode*>(PTR_TO_INT(freeNode) + requiredSize + padding);
				newFreeNode->data.blockSize = restSize;
				newFreeNode->padding = padding;
				newFreeNode->next = nullptr;

				m_freeList.insert(freeNode, newFreeNode);
			}

			m_freeList.remove(previousNode, freeNode);

			// Setup data block
			const char freeNodePadding = freeNode->padding;
			const std::size_t headerAddress = PTR_TO_INT(freeNode);
			AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(headerAddress);
			allocationHeader->blockSize = requiredSize;
			allocationHeader->padding = freeNodePadding;

			const std::size_t dataAddress = headerAddress + cAllocationHeaderSize;			

			m_used += requiredSize;

			return (void*)dataAddress;
		}

		bool Free(void* ptr) override
		{
			// Insert it in a sorted position by the address number
			const std::size_t currentAddress = PTR_TO_INT(ptr);
			const std::size_t headerAddress = currentAddress - cAllocationHeaderSize;

			const AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(headerAddress);
			const std::size_t blockSize = allocationHeader->blockSize;
			const char padding = allocationHeader->padding;

			FreeNode* freeNode = reinterpret_cast<FreeNode*>(headerAddress);

			freeNode->data.blockSize = blockSize;
			freeNode->padding = padding;
			freeNode->next = nullptr;

			FreeNode* it = m_freeList.head;
			FreeNode* itPrev = nullptr;
			while (it != nullptr)
			{
				if (ptr < it)
				{
					m_freeList.insert(itPrev, freeNode);
					break;
				}
				itPrev = it;
				it = it->next;
			}

			m_used -= freeNode->data.blockSize;

			// Merge contiguous nodes
			Coalescence(itPrev, freeNode);

			return true;
		}

		void Reset()
		{
			m_used = 0;

			FreeNode* firstNode = static_cast<FreeNode*>(m_start_ptr);

			firstNode->data.blockSize = m_totalSize;
			firstNode->next = nullptr;
			firstNode->padding = 0;

			m_freeList.head = nullptr;
			m_freeList.insert(nullptr, firstNode);
		}

	private:
		void Coalescence(FreeNode* previousNode, FreeNode* freeNode)
		{
			if (freeNode->next != nullptr &&
				PTR_TO_INT(freeNode) + freeNode->data.blockSize == PTR_TO_INT(freeNode->next) - freeNode->next->padding)
			{
				freeNode->data.blockSize += freeNode->next->data.blockSize + freeNode->next->padding;
				m_freeList.remove(freeNode, freeNode->next);
			}

			if (previousNode != nullptr &&
				PTR_TO_INT(previousNode) + previousNode->data.blockSize == PTR_TO_INT(freeNode) - freeNode->padding)
			{
				previousNode->data.blockSize += freeNode->data.blockSize + freeNode->padding;
				m_freeList.remove(previousNode, freeNode);
			}
		}

		void Find(const std::size_t size, FreeNode*& previousNode, FreeNode*& foundNode) const
		{
			switch (m_pPolicy)
			{
			case FIND_FIRST:
				FindFirst(size, previousNode, foundNode);
				break;
			case FIND_BEST:
				FindBest(size, previousNode, foundNode);
				break;
			}
		}

		void FindFirst(const std::size_t size, FreeNode*& previousNode, FreeNode*& foundNode) const
		{
			//Iterate list and return the first free block with a size >= than given size
			FreeNode *it = m_freeList.head, *itPrev = nullptr;

			while (it != nullptr)
			{
				if (it->data.blockSize >= size)
				{
					break;
				}

				itPrev = it;
				it = it->next;
			}
			previousNode = itPrev;
			foundNode = it;
		}

		void FindBest(const std::size_t size, FreeNode*& previousNode, FreeNode*& foundNode) const
		{
			// Iterate WHOLE list keeping a pointer to the best fit
			const std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
			FreeNode* bestBlock = nullptr;
			FreeNode *it = m_freeList.head,
			     *itPrev = nullptr;

			while (it != nullptr)
			{
				if (it->data.blockSize >= size && (it->data.blockSize - size < smallestDiff))
				{
					bestBlock = it;
				}

				itPrev = it;
				it = it->next;
			}
			previousNode = itPrev;
			foundNode = bestBlock;
		}

	private:
		void* m_start_ptr = nullptr;
		PlacementPolicy m_pPolicy;
		SinglyLinkedList<FreeHeader> m_freeList;
	};
}
