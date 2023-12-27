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

			m_start_ptr = malloc(m_totalSize);

			m_offset = 0;
		}

		~LinearAllocator() override
		{
			free(m_start_ptr);
			m_start_ptr = nullptr;
		}

		void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) override
		{
			std::size_t padding = 0;

			const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

			if (alignment != 0 && m_offset % alignment != 0)
			{
				// Alignment is required. Find the next aligned memory address and update offset
				padding = CalculatePadding(currentAddress, alignment);
			}

			if (m_offset + padding + size > m_totalSize)
			{
				return nullptr;
			}

			m_offset += padding;

			const std::size_t dataAddress = currentAddress + padding;

			m_offset += size;

			m_used = m_offset;

			assert(dataAddress % alignment == 0 && "Data address must be aligment");

			return (void*)dataAddress;
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
		void* m_start_ptr = nullptr;
		std::size_t m_offset = 0;
	};
}
