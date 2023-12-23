#pragma once

#include "AllocatorInterface.h"
#include <cassert>

namespace MemAlloc
{
	class StackAllocator : public AllocatorInterface
	{
	public:
		StackAllocator(StackAllocator& stackAllocator) = delete;

		StackAllocator(const std::size_t totalSize)
			: AllocatorInterface(totalSize)
		{
		}

		void Init() override
		{
			if (m_start_ptr != nullptr)
			{
				free(m_start_ptr);
			}
			m_start_ptr = malloc(m_totalSize);
			m_offset = 0;
		}

		~StackAllocator() override
		{
			free(m_start_ptr);
			m_start_ptr = nullptr;
		}

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			const std::size_t padding = size % alignment;
			const std::size_t requiredSize = m_offset + padding + size;

			assert(requiredSize <= m_totalSize && "The pool allocator is full");
			if (requiredSize > m_totalSize)
			{
				return nullptr;
			}

			void* freeChunk = reinterpret_cast<void*>(PTR_TO_INT(m_start_ptr) + m_offset);

			m_offset += padding + size;
			m_used = m_offset;

			return freeChunk;
		}

		bool Free(void* ptr) override
		{
			// Move offset back to clear address
			m_used = m_offset = (PTR_TO_INT(ptr) - PTR_TO_INT(m_start_ptr));

			return true;
		}

		void Reset()
		{
			m_offset = 0;
			m_used = 0;
		}

	protected:
		void* m_start_ptr = nullptr;
		std::size_t m_offset = 0;
	};
}
