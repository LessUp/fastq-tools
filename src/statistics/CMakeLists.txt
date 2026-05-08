add_library(fq_statistics STATIC
    factory.cpp
    fq_statistic.cpp
    fq_statistic_worker.cpp
    statistics_report.cpp
    statistics_writer.cpp
)

# fq_common PUBLIC 传递 include 路径，无需重复声明 PUBLIC include
target_include_directories(fq_statistics
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# spdlog/TBB 仅在 .cpp 实现中使用，设为 PRIVATE
target_link_libraries(fq_statistics
    PUBLIC
        fq_common
        fq_error
        fq_modern_io
    PRIVATE
        spdlog::spdlog
        TBB::tbb
)

target_compile_features(fq_statistics PUBLIC cxx_std_23)
