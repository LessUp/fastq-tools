enable_testing()

# ---------------------------------------------------------------------------
# add_fq_test(<name> <label> <timeout> <source_files...>)
#
# 注册一个 GTest 测试可执行文件。
#   name    — 目标名称（同时也是 CTest 用例名）
#   label   — CTest 标签（unit / integration）
#   timeout — 超时秒数
#   ARGN    — 源文件列表
# ---------------------------------------------------------------------------
function(add_fq_test test_name label timeout)
    add_executable(${test_name} ${ARGN})

    target_link_libraries(${test_name} PRIVATE
        GTest::gtest_main
        GTest::gmock
        fq_lib
        test_utils
    )

    # fq_lib 已传播 include/；src/ 用于访问内部实现头文件
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    add_test(NAME ${test_name} COMMAND ${test_name})

    set_tests_properties(${test_name} PROPERTIES
        LABELS "${label}"
        TIMEOUT ${timeout}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# --- 子目录 ---
add_subdirectory(utils)
add_subdirectory(unit)
add_subdirectory(integration)

add_test(
    NAME e2e_shell_cli
    COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e/test_cli.sh
)
set_tests_properties(e2e_shell_cli PROPERTIES
    LABELS "e2e"
    TIMEOUT 180
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    ENVIRONMENT "FASTQTOOLS=${CMAKE_BINARY_DIR}/FastQTools"
)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(
    NAME e2e_python_cli
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/e2e/test_advanced_cli.py
)
set_tests_properties(e2e_python_cli PROPERTIES
    LABELS "e2e"
    TIMEOUT 180
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    ENVIRONMENT "FASTQTOOLS=${CMAKE_BINARY_DIR}/FastQTools"
)
