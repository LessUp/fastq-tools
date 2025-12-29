# 单元测试配置

# 通用测试设置函数
function(add_unit_test test_name)
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
    
    # 添加到CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # 设置测试属性
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 60
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Common模块测试
add_unit_test(test_common
    common/test_timer.cpp
    common/test_common.cpp
)

# Config模块测试
add_unit_test(test_config
    config/test_configuration.cpp
    config/test_config_parsing.cpp
)

# Error模块测试
add_unit_test(test_error
    error/test_exceptions.cpp
    error/test_error_handler.cpp
)

# IO模块测试
add_unit_test(test_io
    io/test_fastq_reader.cpp
    io/test_writer.cpp
)

# Processing模块测试
add_unit_test(test_processing
    processing/test_pipeline_smoke.cpp
)

# Statistics模块测试
add_unit_test(test_statistics
    statistics/test_statistics.cpp
)

# 创建测试组
add_custom_target(unit_tests
    DEPENDS 
        test_common
        test_config
        test_error
        test_io
        test_processing
        test_statistics
    COMMENT "Building all unit tests"
)
