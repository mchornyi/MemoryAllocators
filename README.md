# MemoryAllocators
Simple memory allocators with tests

* LinerAllocator
* StackAllocator
* PoolAllocatorMulti
* PoolAllocator
* MallocAllocator
* FreeListAllocator

```
StartTest: FreeListAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 44011786ns
Test Passed!

StartTest: LinerAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 94607ns
Test Passed!

StartTest: MallocAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 16728098ns
Test Passed!

StartMultiThreadTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Create 2 threads. Each thread allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 4152826ns
Test Passed!

StartTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 6670787ns
Test Passed!

StartTest: StackAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 618224ns
Test Passed!

Test results:
LinerAllocator    : 94607 ns
StackAllocator    : 618224 ns
PoolAllocatorMulti: 4152826 ns
PoolAllocator     : 6670787 ns
MallocAllocator   : 16728098 ns
FreeListAllocator : 44011786 ns
```
