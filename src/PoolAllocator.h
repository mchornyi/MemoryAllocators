#pragma once

#include "AllocatorInterface.h"
#include <cassert>

namespace MemAlloc
{
	class PoolAllocator final : public AllocatorInterface
	{
	public:
		PoolAllocator() = delete;

		PoolAllocator(const PoolAllocator& poolAllocator) : AllocatorInterface(poolAllocator)
		{
			m_chunkSize = poolAllocator.m_chunkSize;
			m_chunksNum = poolAllocator.m_chunksNum;
			m_freeList = poolAllocator.m_freeList;
			m_start_ptr = poolAllocator.m_start_ptr;
		}

		PoolAllocator(const std::size_t chunksNum, const std::size_t chunkSize)
			: AllocatorInterface(chunksNum*chunkSize), m_chunksNum(chunksNum), m_chunkSize(chunkSize)
		{
			assert(((chunkSize % sizeof(std::size_t))==0) && "Chunk size must be aligned to std::size_t");
			assert(m_totalSize % chunkSize == 0 && "Total Size must be a multiple of Chunk Size");
		}

		void Init() override
		{
			m_start_ptr = malloc(m_totalSize);
			Reset();
		}

		~PoolAllocator() override
		{
			free(m_start_ptr);
		}

		void* Allocate(const std::size_t allocationSize, const std::size_t alignment = 8) override
		{
			assert(allocationSize <= m_chunkSize && "Allocation size must be <= to chunk size");

			SpinlockGuard guard(m_spinlock);

			Node* freeNode = m_freeList.pop();

			assert(freeNode != nullptr && "The pool allocator is full");
			if (freeNode)
			{
				m_used += m_chunkSize;
			}

			return (void*)freeNode;
		}

		bool Free(void* ptr) override
		{
			SpinlockGuard guard(m_spinlock);

			if (PTR_TO_INT(ptr) < PTR_TO_INT(m_start_ptr) || PTR_TO_INT(ptr) >
				PTR_TO_INT(m_start_ptr) + m_totalSize)
			{
				return false;
			}

			m_used -= m_chunkSize;

			m_freeList.push(static_cast<Node*>(ptr));

			return true;
		}

		void Reset()
		{
			SpinlockGuard guard(m_spinlock);

			m_used = 0;
			// Create a linked-list with all free positions
			const std::size_t nChunks = m_totalSize / m_chunkSize;
			for (std::size_t i = 0; i < nChunks; ++i)
			{
				const std::size_t address = reinterpret_cast<std::size_t>(m_start_ptr) + static_cast<
					std::size_t>(i) * m_chunkSize;

				m_freeList.push(reinterpret_cast<Node*>(address));
			}
		}

		std::size_t GetChunkSize() const
		{
			return m_chunkSize;
		}

	private:
		struct FreeHeader
		{
		};

		using Node = StackLinkedList<FreeHeader>::Node;
		StackLinkedList<FreeHeader> m_freeList;
		void* m_start_ptr = nullptr;
		std::size_t m_chunkSize = 0;
		std::size_t m_chunksNum = 0;
		Spinlock m_spinlock;
	};
} // namespace MemAlloc
