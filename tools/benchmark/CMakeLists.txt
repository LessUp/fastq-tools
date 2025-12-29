# FastQTools 基准测试

cmake_minimum_required(VERSION 3.20)

# 查找 Google Benchmark
find_package(benchmark CONFIG REQUIRED)

# 通用链接库：Google Benchmark + 项目主库
set(BENCHMARK_COMMON_LIBS
    benchmark::benchmark
    benchmark::benchmark_main
    fq_lib
)

# 基准测试工具函数
function(add_benchmark benchmark_name source_files)
    add_executable(${benchmark_name} ${source_files})
    
    target_link_libraries(${benchmark_name} PRIVATE
        ${BENCHMARK_COMMON_LIBS}
    )
    
    target_include_directories(${benchmark_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # 设置基准测试属性
    set_target_properties(${benchmark_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endfunction()

# FastQ 读写基准测试（基于新 IO）
add_benchmark(benchmark_fastq_io
    fastq_io_benchmark.cpp
)

# 如后续需要，可在此处新增其他基准测试（statistics/encoder/memory 等）

# 端到端性能基准测试
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/performance_benchmark.cpp")
    add_executable(performance_benchmark
        performance_benchmark.cpp
    )

    target_link_libraries(performance_benchmark PRIVATE
        fq_cli
        fmt::fmt
        spdlog::spdlog
    )

    target_include_directories(performance_benchmark PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# 创建基准测试组
add_custom_target(benchmarks
    DEPENDS 
        benchmark_fastq_io
    COMMENT "Building all benchmarks"
)

# 运行所有基准测试的目标
add_custom_target(run_benchmarks
    COMMAND benchmark_fastq_io --benchmark_format=json --benchmark_out=fastq_io_results.json
    DEPENDS benchmarks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all benchmarks"
)
