# benchmark 数据收集与结果存储模块
# 依赖 nlohmann_json（通过 benchmark/types.h），由根 CMakeLists.txt find_package
add_library(fq_benchmark STATIC
    data_collector.cpp
    result_storage.cpp
)

target_include_directories(fq_benchmark
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_benchmark
    PUBLIC
        nlohmann_json::nlohmann_json
)

target_compile_features(fq_benchmark PUBLIC cxx_std_23)
