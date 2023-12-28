#include "Test.h"
#include "benchmark/benchmark.h"

#define ENABLE_BENCHMARK 1

using namespace MemAlloc;

int main(int argc, char** argv)
{
	Test::Run();

#if ENABLE_BENCHMARK
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();
#endif

    return 0;
}
