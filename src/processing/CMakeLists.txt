add_library(fq_processing STATIC
    factory.cpp
    processing_pipeline.cpp
    processing_statistics.cpp
    mutators/quality_trimmer.cpp
    predicates/min_quality_predicate.cpp
)

# fq_common PUBLIC 传递 include 路径，无需重复声明 PUBLIC include
target_include_directories(fq_processing
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# fq_modern_io: 公共头文件引用了 FastqBatch 等类型，需 PUBLIC
# fq_common/fq_error: 公共头文件间接引用，需 PUBLIC
# spdlog/TBB/fmt: 仅在 .cpp 实现中使用，设为 PRIVATE
target_link_libraries(fq_processing
    PUBLIC
        fq_common
        fq_error
        fq_modern_io
    PRIVATE
        spdlog::spdlog
        TBB::tbb
        fmt::fmt
)

target_compile_features(fq_processing PUBLIC cxx_std_20)
