/**
 * @file object_pool_benchmark.cpp
 * @brief ObjectPool 对象池性能基准测试
 * @details 验证对象池在高频借出/归还场景下的性能表现
 *
 * @author FastQTools Team
 * @date 2025
 * @copyright Copyright (c) 2025 FastQTools
 */

#include <benchmark/benchmark.h>
#include <fqtools/memory/object_pool.h>
#include <fqtools/io/fastq_io.h>
#include <fqtools/io/fastq_batch_pool.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace fq::benchmark {

// 简单对象：用于测试基础性能
struct SimpleObject {
    std::string data;
    std::vector<int> numbers;

    void reset() {
        data.clear();
        numbers.clear();
    }
};

// Benchmark: 无池化 - 直接 new/delete
static void BM_NoPool_NewDelete(::benchmark::State& state) {
    const std::size_t iterations = static_cast<std::size_t>(state.range(0));

    for (auto _ : state) {
        for (std::size_t i = 0; i < iterations; ++i) {
            auto obj = std::make_unique<SimpleObject>();
            obj->data = "test_data_" + std::to_string(i);
            obj->numbers.resize(100, static_cast<int>(i));
            ::benchmark::DoNotOptimize(obj->data);
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(iterations));
}

// Benchmark: 使用 ObjectPool
static void BM_ObjectPool_AcquireRelease(::benchmark::State& state) {
    const std::size_t iterations = static_cast<std::size_t>(state.range(0));

    auto pool = std::make_shared<fq::memory::ObjectPool<SimpleObject>>(
        4,   // 初始大小
        16,  // 最大大小
        [](SimpleObject& obj) { obj.reset(); });

    for (auto _ : state) {
        for (std::size_t i = 0; i < iterations; ++i) {
            auto obj = pool->acquire();
            obj->data = "test_data_" + std::to_string(i);
            obj->numbers.resize(100, static_cast<int>(i));
            ::benchmark::DoNotOptimize(obj->data);
            // obj 离开作用域时自动归还
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(iterations));
    state.counters["pool_created"] = static_cast<double>(pool->totalCreated());
}

// Benchmark: FastqBatch 无池化
static void BM_FastqBatch_NoPool(::benchmark::State& state) {
    const std::size_t iterations = static_cast<std::size_t>(state.range(0));
    constexpr std::size_t kBatchCapacity = 4 * 1024 * 1024;  // 4MB

    for (auto _ : state) {
        for (std::size_t i = 0; i < iterations; ++i) {
            auto batch = std::make_unique<fq::io::FastqBatch>(kBatchCapacity);
            batch->buffer().reserve(kBatchCapacity);
            ::benchmark::DoNotOptimize(batch->buffer().capacity());
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(iterations));
}

// Benchmark: FastqBatch 使用池化
static void BM_FastqBatch_WithPool(::benchmark::State& state) {
    const std::size_t iterations = static_cast<std::size_t>(state.range(0));

    auto pool = fq::io::createFastqBatchPool(4, 16);

    for (auto _ : state) {
        for (std::size_t i = 0; i < iterations; ++i) {
            auto batch = pool->acquire();
            // 模拟使用
            batch->buffer().reserve(4 * 1024 * 1024);
            ::benchmark::DoNotOptimize(batch->buffer().capacity());
            // batch 离开作用域时自动归还
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(iterations));
    state.counters["pool_created"] = static_cast<double>(pool->totalCreated());
}

// Benchmark: 多线程并发获取/归还
static void BM_ObjectPool_Concurrent(::benchmark::State& state) {
    const std::size_t num_threads = static_cast<std::size_t>(state.range(0));
    const std::size_t ops_per_thread = 100;

    auto pool = std::make_shared<fq::memory::ObjectPool<SimpleObject>>(
        num_threads,      // 初始大小
        num_threads * 2,  // 最大大小
        [](SimpleObject& obj) { obj.reset(); });

    for (auto _ : state) {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (std::size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([pool, ops_per_thread, t]() {
                for (std::size_t i = 0; i < ops_per_thread; ++i) {
                    auto obj = pool->acquire();
                    obj->data = "thread_" + std::to_string(t) + "_op_" + std::to_string(i);
                    ::benchmark::DoNotOptimize(obj->data);
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_threads * ops_per_thread));
    state.counters["threads"] = static_cast<double>(num_threads);
    state.counters["pool_created"] = static_cast<double>(pool->totalCreated());
}

// 注册 benchmark
BENCHMARK(BM_NoPool_NewDelete)
    ->Args({100})
    ->Args({1000})
    ->Args({10000})
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK(BM_ObjectPool_AcquireRelease)
    ->Args({100})
    ->Args({1000})
    ->Args({10000})
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK(BM_FastqBatch_NoPool)
    ->Args({10})
    ->Args({100})
    ->Args({1000})
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK(BM_FastqBatch_WithPool)
    ->Args({10})
    ->Args({100})
    ->Args({1000})
    ->Unit(::benchmark::kMicrosecond);

BENCHMARK(BM_ObjectPool_Concurrent)
    ->Args({2})
    ->Args({4})
    ->Args({8})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
