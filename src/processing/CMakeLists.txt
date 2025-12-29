add_library(fq_processing STATIC
    factory.cpp
    processing_pipeline.cpp
    processing_statistics.cpp
    mutators/quality_trimmer.cpp
    predicates/min_quality_predicate.cpp
)

target_include_directories(fq_processing
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fq_processing
    PUBLIC
        fq_core
        fq_modern_io
        spdlog::spdlog
        TBB::tbb
        fmt::fmt
)

target_compile_features(fq_processing PUBLIC cxx_std_20)
