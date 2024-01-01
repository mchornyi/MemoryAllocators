#pragma once

#include "AllocatorInterface.h"
#include <cassert>

namespace MemAlloc
{
	const ThreadPolicy cPoolAllocThreadPolicy(NONE);

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
			m_start_ptr = static_cast<char*>(malloc(m_totalSize));
			m_freeChunks = static_cast<char**>(malloc(m_chunksNum * sizeof(char*)));

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

			switch (cPoolAllocThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.lock();
				break;
			case NONE:
				break;
			}

			assert(m_currFreeChunksIdx >= 0 && "The pool allocator is full");			
			if (m_currFreeChunksIdx < 0)
			{
				return nullptr;
			}

			void* dataAddress = m_freeChunks[m_currFreeChunksIdx--];
			
			m_used += m_chunkSize;

			assert(PTR_TO_INT(dataAddress) % alignment == 0 && "Data address must be aligment");

			switch (cPoolAllocThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.unlock();
				break;
			case NONE:
				break;
			}

			return dataAddress;
		}

		bool Free(void* ptr) override
		{
			switch (cPoolAllocThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.lock();
				break;
			case NONE:
				break;
			}

			if (ptr < m_start_ptr || ptr > m_start_ptr + m_totalSize)
			{
				return false;
			}

			m_used -= m_chunkSize;

			m_freeChunks[++m_currFreeChunksIdx] = static_cast<char*>(ptr);

			switch (cPoolAllocThreadPolicy)
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
			switch (cPoolAllocThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.lock();
				break;
			case NONE:
				break;
			}

			m_used = 0;

			for (std::size_t i = 0; i < m_chunksNum; ++i)
			{
				m_freeChunks[i] = m_start_ptr + (i)*m_chunkSize;
			}

			m_currFreeChunksIdx = static_cast<int64_t>(m_chunksNum) - 1;

			switch (cPoolAllocThreadPolicy)
			{
			case ENABLE_SPIN_LOCK:
				m_spinlock.unlock();
				break;
			case NONE:
				break;
			}
		}

		std::size_t GetChunkSize() const
		{
			return m_chunkSize;
		}

	private:
		char** m_freeChunks = nullptr;
		char* m_start_ptr = nullptr;
		std::size_t m_chunksNum = 0;
		std::size_t m_chunkSize = 0;
		int64_t m_currFreeChunksIdx = -1;
		Spinlock m_spinlock;
	};
} // namespace MemAlloc
