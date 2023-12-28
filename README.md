# MemoryAllocators

Simple memory allocators with tests

* LinerAllocator
* StackAllocator
* PoolAllocator
* PoolAlloc2Threads
* FreeListAllocator
* MallocAllocator

```text
StartTest: FreeListAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 23960200ns
Test Passed!

StartTest: LinerAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 355700ns
Test Passed!

StartTest: MallocAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 44523200ns
Test Passed!

StartMultiThreadTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Create 2 threads. Each thread allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 11898500ns
Test Passed!

StartTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 7636200ns
Test Passed!

StartTest: StackAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 1633400ns
Test Passed!

Test results:
LinerAllocator    : 355700 ns
StackAllocator    : 1633400 ns
PoolAllocator     : 7636200 ns
PoolAlloc2Threads : 11898500 ns
FreeListAllocator : 23960200 ns
MallocAllocator   : 44523200 ns

2023-12-28T18:13:12+02:00
Running D:\projects\MemoryAllocators\bin\Release\main.exe
Run on (32 X 3394 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 512 KiB (x16)
  L3 Unified 32768 KiB (x2)
---------------------------------------------------------------------------
Benchmark                 Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------
BM_StackAlloc          1.54 ns         1.53 ns    560000000 bytes_per_second=621.449Mi/s
BM_LinerAlloc          2.19 ns         2.15 ns    320000000 bytes_per_second=443.892Mi/s
BM_PoolAlloc           16.9 ns         16.9 ns     40727273 bytes_per_second=56.4954Mi/s
BM_FreeListAlloc       30.1 ns         30.5 ns     23578947 bytes_per_second=31.2858Mi/s
BM_MallocAlloc          108 ns          108 ns      8960000 bytes_per_second=8.82056Mi/s
```
