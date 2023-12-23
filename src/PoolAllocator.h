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
			m_start_ptr = poolAllocator.m_start_ptr;
			m_freeChunks = poolAllocator.m_freeChunks;
			m_chunkSize = poolAllocator.m_chunkSize;
			m_chunksNum = poolAllocator.m_chunksNum;
			m_currFreeChunksIdx = poolAllocator.m_currFreeChunksIdx;
		}

		PoolAllocator(const std::size_t chunksNum, const std::size_t chunkSize)
			: AllocatorInterface(chunksNum*chunkSize), m_chunksNum(chunksNum), m_chunkSize(chunkSize), m_currFreeChunksIdx(m_chunksNum -1)
		{
			assert(((chunkSize % sizeof(std::size_t))==0) && "Chunk size must be aligned to std::size_t");
			assert(m_totalSize % chunkSize == 0 && "Total Size must be a multiple of Chunk Size");
		}

		void Init() override
		{
			m_start_ptr = malloc(m_totalSize);
			m_freeChunks = (void**)malloc(m_chunksNum * sizeof(void*));

			Reset();
		}

		~PoolAllocator() override
		{
			free(m_start_ptr);
			free(m_freeChunks);
		}

		void* Allocate(const std::size_t allocationSize, const std::size_t alignment = sizeof(std::size_t)) override
		{
			assert(allocationSize <= m_chunkSize && "Allocation size must be <= to chunk size");

			SpinlockGuard guard(m_spinlock);

			assert(m_currFreeChunksIdx >= 0 && "The pool allocator is full");			
			if (m_currFreeChunksIdx < 0)
			{
				return nullptr;
			}

			void* freeChunk = m_freeChunks[m_currFreeChunksIdx--];
			
			m_used += m_chunkSize;

			return freeChunk;
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

			m_freeChunks[++m_currFreeChunksIdx] = ptr;

			return true;
		}

		void Reset()
		{
			SpinlockGuard guard(m_spinlock);

			m_used = 0;

			for (std::size_t i = 0; i < m_chunksNum; ++i)
			{
				const std::size_t address = reinterpret_cast<std::size_t>(m_start_ptr) + (i) * m_chunkSize;
				m_freeChunks[i] = reinterpret_cast<void*>(address);
			}

			m_currFreeChunksIdx = m_chunksNum - 1;
		}

		std::size_t GetChunkSize() const
		{
			return m_chunkSize;
		}

	private:
		void** m_freeChunks = nullptr;
		void* m_start_ptr = nullptr;
		int64_t m_currFreeChunksIdx = -1;
		std::size_t m_chunkSize = 0;
		std::size_t m_chunksNum = 0;
		Spinlock m_spinlock;
	};
} // namespace MemAlloc
