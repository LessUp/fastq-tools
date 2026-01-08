# 集成测试配置
# 
# 集成测试验证多个模块之间的交互，测试跨组件的功能
# 与单元测试不同，集成测试可能涉及文件 I/O、多线程等复杂场景

# 通用集成测试设置函数
function(add_integration_test test_name)
    set(source_files ${ARGN})
    add_executable(${test_name} ${source_files})
    
    target_link_libraries(${test_name} PRIVATE
        GTest::gtest_main
        GTest::gtest
        GTest::gmock
        fq_lib
        test_utils
    )
    
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    )
    
    # 添加到 CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # 设置测试属性
    set_tests_properties(${test_name} PROPERTIES
        LABELS "integration"
        TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Pipeline 集成测试
add_integration_test(test_integration_pipeline
    test_pipeline_integration.cpp
)

# 创建集成测试组
add_custom_target(integration_tests
    DEPENDS 
        test_integration_pipeline
    COMMENT "Building all integration tests"
)
