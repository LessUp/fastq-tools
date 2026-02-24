enable_testing()

find_package(GTest REQUIRED)

# 通用测试注册函数
# 参数: test_name label timeout source_files...
function(add_fq_test test_name label timeout)
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
    
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    set_tests_properties(${test_name} PROPERTIES
        LABELS "${label}"
        TIMEOUT ${timeout}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

add_subdirectory(utils)
add_subdirectory(unit)
add_subdirectory(integration)
