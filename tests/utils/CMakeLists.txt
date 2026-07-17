# 测试工具库
# - fixture_loader: 测试数据加载、临时目录管理、文件比较
# - test_helpers:   FASTQ 数据生成、测试基类
add_library(test_utils STATIC
    fixture_loader.cpp
    test_helpers.cpp
)

target_include_directories(test_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_utils
    PUBLIC
        GTest::gtest
)
