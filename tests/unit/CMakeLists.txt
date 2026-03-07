# ---------------------------------------------------------------------------
# 单元测试
# 使用 add_fq_test(name label timeout sources...) 注册
# ---------------------------------------------------------------------------

# --- 基础设施 ---
add_fq_test(test_common     "unit" 60  common/test_timer.cpp common/test_common.cpp)
add_fq_test(test_config     "unit" 60  config/test_configuration.cpp config/test_config_parsing.cpp)
add_fq_test(test_error      "unit" 60  error/test_exceptions.cpp error/test_error_handler.cpp)

# --- 核心功能 ---
add_fq_test(test_io         "unit" 60  io/test_fastq_reader.cpp io/test_writer.cpp)
add_fq_test(test_processing "unit" 60  processing/test_pipeline_smoke.cpp)
add_fq_test(test_statistics "unit" 60  statistics/test_statistics.cpp)
add_fq_test(test_memory     "unit" 60  memory/test_object_pool.cpp)

# --- 聚合目标 ---
add_custom_target(unit_tests
    DEPENDS
        test_common
        test_config
        test_error
        test_io
        test_processing
        test_statistics
        test_memory
)
