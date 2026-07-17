add_library(fq_processing STATIC
    execution_backend.cpp
    execution_runtime.cpp
    one_tbb_execution_backend.cpp
    factory.cpp
    resolved_runtime_config.cpp
    processing_options.cpp
    processing_pipeline.cpp
    processing_statistics.cpp
    taskflow_execution_backend.cpp
    mutators/quality_trimmer.cpp
    predicates/min_quality_predicate.cpp
)

target_include_directories(fq_processing
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# fq_modern_io: 公共头文件引用了 FastqBatch 等类型，需 PUBLIC
# fq_error: 公共头文件间接引用，需 PUBLIC
# 调度框架仅存在于 backend 实现文件，不向消费者暴露。
target_link_libraries(fq_processing
    PUBLIC
        fq_error
        fq_modern_io
    PRIVATE
        fmt::fmt
        TBB::tbb
)

target_compile_features(fq_processing PUBLIC cxx_std_23)

if(ENABLE_TASKFLOW_BACKEND)
    target_compile_definitions(fq_processing PRIVATE FQ_ENABLE_TASKFLOW_BACKEND=1)
    target_link_libraries(fq_processing PRIVATE Taskflow::Taskflow)
endif()
