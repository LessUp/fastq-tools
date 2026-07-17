# ---------------------------------------------------------------------------
# 单元测试
# 使用 add_fq_test(name label timeout sources...) 注册
# ---------------------------------------------------------------------------

# --- 基础设施 ---
add_fq_test(test_error      "unit" 60  error/test_exceptions.cpp)

# --- 核心功能 ---
add_fq_test(test_io         "unit" 60  io/test_fastq_reader.cpp io/test_writer.cpp)
add_fq_test(test_processing "unit" 60  processing/test_pipeline_smoke.cpp processing/test_resolved_runtime_config.cpp processing/test_execution_runtime.cpp)
add_fq_test(test_cli        "unit" 60  cli/test_filter_plan.cpp cli/test_enum_parser.cpp cli/test_command_registry.cpp)
target_link_libraries(test_cli PRIVATE fq_cli cxxopts::cxxopts)
add_fq_test(test_predicates "unit" 60  processing/test_predicates.cpp)
add_fq_test(test_mutators   "unit" 60  processing/test_mutators.cpp)
add_fq_test(test_statistics "unit" 60  statistics/test_statistics.cpp statistics/test_statistics_report.cpp)
add_fq_test(test_memory     "unit" 60  memory/test_object_pool.cpp)

# --- 聚合目标 ---
add_custom_target(unit_tests
    DEPENDS
        test_error
        test_io
        test_processing
        test_predicates
        test_mutators
        test_statistics
        test_memory
)
