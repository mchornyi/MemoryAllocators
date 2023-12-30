#pragma once
#include <atomic>
#include <thread>
#include <iostream>
#define PTR_TO_INT(PTR) (reinterpret_cast<std::size_t>(PTR))
#define PTR_TO_CHAR(PTR) (reinterpret_cast<char*>(PTR))
namespace MemAlloc
{
	class AllocatorInterface
	{
	public:
		AllocatorInterface() = delete;

		AllocatorInterface(const std::size_t totalSize) : m_totalSize{totalSize}
		{
		}

		AllocatorInterface(const AllocatorInterface& allocator)
		{
			m_totalSize = allocator.m_totalSize;
			m_used = allocator.m_used;
		}

		virtual ~AllocatorInterface()
		{
			m_totalSize = 0;
		}

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = sizeof(std::size_t)) = 0;
		virtual bool Free(void* ptr) = 0;
		virtual void Init() = 0;

		std::size_t GetTotalSize() const
		{
			return m_totalSize;
		}

		std::size_t GetUsedSize() const
		{
			return m_used;
		}

	protected:
		std::size_t m_totalSize = 0;
		std::size_t m_used = 0;
	};

	class Spinlock
	{
	public:
		Spinlock() = default;
		Spinlock(const Spinlock&) = delete;
		Spinlock& operator=(const Spinlock&) = delete;

		void lock()
		{
			std::size_t counterToYield = 10;
			while (mFlag.load(std::memory_order_relaxed) || mFlag.exchange(true, std::memory_order_acquire))
			{
				if (--counterToYield == 0)
				{
					counterToYield = 10;
					std::this_thread::yield();
				}
			}
		}

		void unlock()
		{
			mFlag.store(false, std::memory_order_release);
		}

	private:
		std::atomic_bool mFlag{false};
	};

	struct SpinlockGuard
	{
		SpinlockGuard(Spinlock& spinlock) : mSpinlock(spinlock)
		{
			mSpinlock.lock();
		}

		~SpinlockGuard()
		{
			mSpinlock.unlock();
		}

		Spinlock& mSpinlock;
	};

	template <class T>
	class StackLinkedList
	{
	public:
		struct Node
		{
			T data{};
			Node* next = nullptr;
		};

		Node* head = nullptr;

	public:
		StackLinkedList() = default;
		StackLinkedList(StackLinkedList& stackLinkedList) = delete;
		void push(Node* newNode);
		Node* pop();
	};

	template <class T>
	void StackLinkedList<T>::push(Node* newNode)
	{
		newNode->next = head;
		head = newNode;
	}

	template <class T>
	typename StackLinkedList<T>::Node* StackLinkedList<T>::pop()
	{
		if (!head)
		{
			return nullptr;
		}

		Node* top = head;
		head = head->next;
		return top;
	}

	template <class T>
	struct SinglyLinkedList
	{
		struct alignas(sizeof(std::size_t)) Node
		{
			T data;
			Node* next;
			char padding = 0;
		};

		SinglyLinkedList() = default;

		void insert(Node* previousNode, Node* newNode)
		{
			if (previousNode == nullptr)
			{
				// Is the first node
				if (head != nullptr)
				{
					// The list has more elements
					newNode->next = head;
				}
				else
				{
					newNode->next = nullptr;
				}
				head = newNode;
			}
			else
			{
				if (previousNode->next == nullptr)
				{
					// Is the last node
					previousNode->next = newNode;
					newNode->next = nullptr;
				}
				else
				{
					// Is a middle node
					newNode->next = previousNode->next;
					previousNode->next = newNode;
				}
			}
		}

		void remove(Node* previousNode, Node* deleteNode)
		{
			if (previousNode == nullptr)
			{
				// Is the first node
				if (deleteNode->next == nullptr)
				{
					// List only has one element
					head = nullptr;
				}
				else
				{
					// List has more elements
					head = deleteNode->next;
				}
			}
			else
			{
				previousNode->next = deleteNode->next;
			}
		}

		Node* head;
	};

	inline char CalculatePadding(const std::size_t baseAddress, const std::size_t alignment)
	{
		return  static_cast<char>(alignment - baseAddress % alignment);
	}
}
