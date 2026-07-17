add_library(fq_statistics STATIC
    factory.cpp
    fq_statistic.cpp
    fq_statistic_worker.cpp
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
        fq_error
        fq_modern_io
    PRIVATE
        fq_processing
)

target_compile_features(fq_statistics PUBLIC cxx_std_23)
