# FastQTools 基准测试

cmake_minimum_required(VERSION 3.20)

# 查找 Google Benchmark
find_package(benchmark CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 通用链接库：Google Benchmark + 项目主库
set(BENCHMARK_COMMON_LIBS
    benchmark::benchmark
    benchmark::benchmark_main
    fq_lib
    nlohmann_json::nlohmann_json
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

# Filter 命令基准测试
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/filter_benchmark.cpp")
    add_benchmark(benchmark_filter
        filter_benchmark.cpp
    )
endif()

# Stat 命令基准测试
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/stat_benchmark.cpp")
    add_benchmark(benchmark_stat
        stat_benchmark.cpp
    )
endif()

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

# 收集所有基准测试目标
set(ALL_BENCHMARK_TARGETS benchmark_fastq_io)

if (TARGET benchmark_filter)
    list(APPEND ALL_BENCHMARK_TARGETS benchmark_filter)
endif()

if (TARGET benchmark_stat)
    list(APPEND ALL_BENCHMARK_TARGETS benchmark_stat)
endif()

# 创建基准测试组
add_custom_target(benchmarks
    DEPENDS ${ALL_BENCHMARK_TARGETS}
    COMMENT "Building all benchmarks"
)

# 定义输出目录
set(BENCHMARK_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/benchmark_results/results")

# 运行所有基准测试的目标（输出 JSON 格式）
add_custom_target(run_benchmarks
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BENCHMARK_OUTPUT_DIR}
    COMMAND benchmark_fastq_io 
        --benchmark_format=json 
        --benchmark_out=${BENCHMARK_OUTPUT_DIR}/fastq_io_results.json
    DEPENDS benchmarks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all benchmarks with JSON output"
)

# 完整基准测试目标（包含所有测试和报告生成）
add_custom_target(benchmark_full
    COMMAND ${CMAKE_COMMAND} -E echo "Running full benchmark suite..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BENCHMARK_OUTPUT_DIR}
    COMMAND benchmark_fastq_io 
        --benchmark_format=json 
        --benchmark_out=${BENCHMARK_OUTPUT_DIR}/fastq_io_results.json
        --benchmark_repetitions=3
        --benchmark_report_aggregates_only=true
    DEPENDS benchmarks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running full benchmark suite with multiple repetitions"
)

# CI 模式基准测试目标
add_custom_target(benchmark_ci
    COMMAND ${CMAKE_COMMAND} -E echo "Running CI benchmark suite..."
    COMMAND benchmark_fastq_io 
        --benchmark_format=console 
        --benchmark_out=${BENCHMARK_OUTPUT_DIR}/ci_results.json
        --benchmark_out_format=json
    DEPENDS benchmarks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running benchmarks in CI mode"
)
