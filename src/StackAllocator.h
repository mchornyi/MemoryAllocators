#pragma once
#include "AllocatorInterface.h"

namespace MemAlloc
{
	class StackAllocator : public AllocatorInterface
	{
		struct alignas(sizeof(std::size_t)) AllocationHeader
		{
			char padding;
		};

		const std::size_t cAllocationHeaderSize = sizeof(AllocationHeader);

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
			const std::size_t currentAddress = PTR_TO_INT(m_start_ptr) + m_offset;

			const std::size_t padding = CalculatePadding(currentAddress, alignment);

			if (m_offset + padding + cAllocationHeaderSize + size > m_totalSize)
			{
				return nullptr;
			}

			const std::size_t nextAddress = currentAddress + padding;
			AllocationHeader* allocationHeader = reinterpret_cast<AllocationHeader*>(nextAddress);
			allocationHeader->padding = static_cast<char>(padding);

			m_offset += padding + cAllocationHeaderSize + size;
			m_used = m_offset;

			const std::size_t resultAddress = PTR_TO_INT(allocationHeader) + cAllocationHeaderSize;
			return (void*)resultAddress;
		}

		bool Free(void* ptr) override
		{
			// Move offset back to clear address
			const std::size_t currentAddress = PTR_TO_INT(ptr);
			const std::size_t headerAddress = currentAddress - cAllocationHeaderSize;

			const AllocationHeader* allocationHeader{reinterpret_cast<AllocationHeader*>(headerAddress)};

			m_offset = (headerAddress - allocationHeader->padding) - PTR_TO_INT(m_start_ptr);
			m_used = m_offset;

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
