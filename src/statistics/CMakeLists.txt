add_library(fq_statistics STATIC
    fq_statistic.cpp
    statistics_report.cpp
    statistics_writer.cpp
)

target_include_directories(fq_statistics
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fq_statistics
    PUBLIC
        fq_stat_core
        fq_modern_io
        fq_processing
    PRIVATE
        fq_error
        fmt::fmt
)

target_compile_features(fq_statistics PUBLIC cxx_std_23)
