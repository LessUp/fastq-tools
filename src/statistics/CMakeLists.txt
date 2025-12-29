add_library(fq_statistics STATIC
    fq_statistic.cpp
    fq_statistic_worker.cpp
)

target_include_directories(fq_statistics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fq_statistics
    PUBLIC
        fq_core
        fq_modern_io
        spdlog::spdlog
        TBB::tbb
)

target_compile_features(fq_statistics PUBLIC cxx_std_20)
