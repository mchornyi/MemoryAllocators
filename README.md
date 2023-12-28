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
Time = 10715393ns
Test Passed!

StartTest: LinerAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates by resetting.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 93876ns
Test Passed!

StartTest: MallocAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 14356226ns
Test Passed!

StartMultiThreadTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Create 2 threads. Each thread allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 3065385ns
Test Passed!

StartTest: PoolAllocator
Desc: Creates 9 pool allocators of different chunk size. Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in random order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 3929365ns
Test Passed!

StartTest: StackAllocator
Desc: Allocates chunks(MaxChunksNum) of size = 'rand() % sMaxChunkSize + 1'. Deallocates in LIFO order.
MaxChunksNum 10000
MaxChunkSize 5120
Time = 446074ns
Test Passed!

Test results:
LinerAllocator    : 93876 ns
StackAllocator    : 446074 ns
PoolAlloc2Threads : 3065385 ns
PoolAllocator     : 3929365 ns
FreeListAllocator : 10715393 ns
MallocAllocator   : 14356226 ns

2023-12-28T18:35:32+00:00
Running /workspaces/MemoryAllocators/build/bin/main
Run on (2 X 3244.27 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x1)
  L1 Instruction 32 KiB (x1)
  L2 Unified 512 KiB (x1)
  L3 Unified 32768 KiB (x1)
Load Average: 0.43, 0.30, 0.37
---------------------------------------------------------------------------
Benchmark                 Time             CPU   Iterations UserCounters...
---------------------------------------------------------------------------
BM_FreeListAlloc       16.2 ns         15.7 ns     42746499 bytes_per_second=60.7027Mi/s
BM_LinerAlloc         0.344 ns        0.343 ns   1821976735 bytes_per_second=2.71194Gi/s
BM_MallocAlloc         9.62 ns         9.60 ns     69069564 bytes_per_second=99.312Mi/s
BM_PoolAlloc           8.99 ns         8.95 ns     79228720 bytes_per_second=106.54Mi/s
BM_StackAlloc         0.318 ns        0.317 ns   2227509552 bytes_per_second=2.93384Gi/s
```
