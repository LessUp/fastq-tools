add_library(fq_processing STATIC
    execution_backend.cpp
    execution_runtime.cpp
    one_tbb_execution_backend.cpp
    resolved_runtime_config.cpp
    processing_options.cpp
    processing_pipeline.cpp
    processing_statistics.cpp
    mutators/quality_trimmer.cpp
    predicates/min_quality_predicate.cpp
)

target_include_directories(fq_processing
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# fq_modern_io: 公共头文件引用 FastqBatch 等类型，需 PUBLIC
# fq_error: 仅 .cpp 抛异常使用，公共头不依赖，PRIVATE
# TBB: 内部头 object_pool.h 依赖 concurrent_bounded_queue，测试与基准通过 fq_lib 链接链
# 访问它，需 PUBLIC 传播头文件路径
target_link_libraries(fq_processing
    PUBLIC
        fq_modern_io
        TBB::tbb
    PRIVATE
        fq_stat_core
        fq_error
        fmt::fmt
)

target_compile_features(fq_processing PUBLIC cxx_std_23)
