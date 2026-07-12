# benchmark 数据收集与结果存储模块（内部使用，不作为公共 API 导出）
# 依赖 nlohmann_json（通过 benchmark/types.h），由根 CMakeLists.txt find_package
add_library(fq_benchmark STATIC
    data_collector.cpp
    result_storage.cpp
)

# 头文件已下沉到本目录，作为内部头处理
target_include_directories(fq_benchmark
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fq_benchmark
    PUBLIC
        nlohmann_json::nlohmann_json
)

target_compile_features(fq_benchmark PUBLIC cxx_std_23)
