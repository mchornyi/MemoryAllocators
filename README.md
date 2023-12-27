# MemoryAllocators

Simple memory allocators with tests

* LinerAllocator
* StackAllocator
* PoolAllocatorMulti
* PoolAllocator
* MallocAllocator
* FreeListAllocator

```text
StartTest: FreeListAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 19772093ns
Test Passed!

StartTest: LinerAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 137766ns
Test Passed!

StartTest: MallocAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 18222547ns
Test Passed!

StartMultiThreadTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Create 2 threads. Each thread allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 11513949ns
Test Passed!

StartTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 5011657ns
Test Passed!

StartTest: StackAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 788759ns
Test Passed!

Test results:
LinerAllocator    : 137766 ns
StackAllocator    : 788759 ns
PoolAllocator     : 5011657 ns
PoolAlloc2Threads : 11513949 ns
MallocAllocator   : 18222547 ns
FreeListAllocator : 19772093 ns
```
