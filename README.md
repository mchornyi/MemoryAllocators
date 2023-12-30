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
MaxChunksNum 1000
MaxChunkSize 5120
Time = 426396ns
Test Passed!
FullMerge Test Passed!

StartTest: LinerAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.
MaxChunksNum 1000
MaxChunkSize 5120
Time = 12392ns
Test Passed!

StartTest: MallocAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 1000
MaxChunkSize 5120
Time = 1123828ns
Test Passed!


StartTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 1000
MaxChunkSize 5120
Time = 99175ns
Test Passed!

StartTest: StackAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.
MaxChunksNum 1000
MaxChunkSize 5120
Time = 66033ns
Test Passed!

Test results:
LinerAllocator    : 12392 ns
StackAllocator    : 66033 ns
PoolAllocator     : 99175 ns
FreeListAllocator : 426396 ns
MallocAllocator   : 1123828 ns

2023-12-30T15:12:03+00:00
Running /workspaces/MemoryAllocators/build/bin/main
Run on (2 X 3242.64 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x1)
  L1 Instruction 32 KiB (x1)
  L2 Unified 512 KiB (x1)
  L3 Unified 32768 KiB (x1)
Load Average: 0.86, 0.69, 0.57
---------------------------------------------------------------------------
Benchmark                 Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------
BM_FreeListAlloc       7.55 ns         7.10 ns    130255095 bytes_per_second=134.274Mi/s
BM_LinerAlloc         0.405 ns        0.378 ns   2069858921 bytes_per_second=2.46647Gi/s
BM_MallocAlloc         9.89 ns         9.88 ns     63633645 bytes_per_second=96.4829Mi/s
BM_PoolAlloc           2.08 ns         2.08 ns    324106785 bytes_per_second=459.361Mi/s
BM_StackAlloc         0.318 ns        0.318 ns   1651904440 bytes_per_second=2.93306Gi/s
```
