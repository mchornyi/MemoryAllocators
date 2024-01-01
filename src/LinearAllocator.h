#pragma once
#include <cassert>

#include "AllocatorInterface.h"

namespace MemAlloc
{
	class LinearAllocator : public AllocatorInterface
	{
	public:
		LinearAllocator(LinearAllocator& linearAllocator) = delete;

		LinearAllocator(const std::size_t totalSize)
			: AllocatorInterface(totalSize)
		{
		}

		void Init() override
		{
			if (m_start_ptr != nullptr)
			{
				free(m_start_ptr);
			}

			m_start_ptr = reinterpret_cast<char*>(malloc(m_totalSize));

			m_offset = 0;
		}

		~LinearAllocator() override
		{
			free(m_start_ptr);
			m_start_ptr = nullptr;
		}

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			const std::size_t padding = alignment - size % alignment;
			const std::size_t requiredSize = m_offset + padding + size;

			assert(requiredSize <= m_totalSize && "The pool allocator is full");
			if (requiredSize > m_totalSize)
			{
				return nullptr;
			}

			void* dataAddress = m_start_ptr + m_offset;

			m_offset += padding + size;
			m_used = m_offset;

			assert(PTR_TO_INT(dataAddress) % alignment == 0 && "Data address must be aligment");

			return dataAddress;
		}

		bool Free(void* ptr) override
		{
			assert(false && "Use Reset() method");
			return false;
		}

		void Reset()
		{
			free(m_start_ptr);

			m_start_ptr = nullptr;
			m_offset = 0;
			m_used = 0;
		}

	protected:
		char* m_start_ptr = nullptr;
		std::size_t m_offset = 0;
	};
}
