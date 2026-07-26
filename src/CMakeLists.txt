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
# 显式列出全部组件（含已被传递的底层库 fq_error/fq_modern_io），保证聚合依赖可见性
add_library(fq_runtime INTERFACE)
set_target_properties(fq_runtime PROPERTIES EXPORT_NAME FastQTools)

target_link_libraries(fq_runtime
    INTERFACE
        fq_error
        fq_modern_io
        fq_processing
        fq_statistics
)

target_include_directories(fq_runtime
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Internal compatibility name; never installed or exported.
add_library(fq_lib ALIAS fq_runtime)
