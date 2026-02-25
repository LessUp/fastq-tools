# Add components directly
add_subdirectory(common)
add_subdirectory(error)
add_subdirectory(config)

add_subdirectory(processing)
add_subdirectory(statistics)
add_subdirectory(io)
add_subdirectory(benchmark)
add_subdirectory(cli)

# 聚合库：将所有组件统一为一个 INTERFACE target，供 CLI 层链接
# 依赖传递关系：fq_config → fq_error → fq_common（→ fmt::fmt）
# fq_processing / fq_statistics → fq_modern_io, fq_common, fq_error
# 因此只需列出叶子模块，无需重复列出已被传递链接的 target
add_library(fq_lib INTERFACE)

target_link_libraries(fq_lib
    INTERFACE
        fq_config
        fq_modern_io
        fq_processing
        fq_statistics
)

target_include_directories(fq_lib
    INTERFACE
        ${CMAKE_SOURCE_DIR}/include
)
