# 集成测试配置
# 使用父级 tests/CMakeLists.txt 中定义的 add_fq_test(name label timeout sources...)
#
# 集成测试验证多个模块之间的交互，测试跨组件的功能
# 与单元测试不同，集成测试可能涉及文件 I/O、多线程等复杂场景

# Pipeline 集成测试
add_fq_test(test_integration_pipeline "integration" 120
    test_pipeline_integration.cpp
)

# 创建集成测试组
add_custom_target(integration_tests
    DEPENDS 
        test_integration_pipeline
    COMMENT "Building all integration tests"
)
