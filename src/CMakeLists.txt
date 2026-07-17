# Add core components
add_subdirectory(error)

add_subdirectory(io)
add_subdirectory(processing)
add_subdirectory(statistics)
add_subdirectory(cli)

# benchmark 数据收集模块仅在需要时编译
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

# 聚合库：将所有组件统一为一个 INTERFACE target，供 CLI 层链接
# fq_processing / fq_statistics → fq_modern_io, fq_error
# 因此只需列出叶子模块，无需重复列出已被传递链接的 target
add_library(fq_lib INTERFACE)

target_link_libraries(fq_lib
    INTERFACE
        fq_error
        fq_modern_io
        fq_processing
        fq_statistics
)

target_include_directories(fq_lib
    INTERFACE
        ${CMAKE_SOURCE_DIR}/include
)
