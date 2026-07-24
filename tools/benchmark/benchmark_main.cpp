#include "benchmark_support.h"

#include <benchmark/benchmark.h>

auto main(int argc, char** argv) -> int {
    ::benchmark::Initialize(&argc, argv);
    fq::benchmark::registerSystemContext();
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}