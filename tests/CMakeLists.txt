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

    # TSan 构建时使用自定义 main（含 TBB 调度器清理），避免进程退出挂起
    if(ENABLE_TSAN)
        target_sources(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/tests/utils/tbb_finalizer.cpp)
        target_link_libraries(${test_name} PRIVATE
            GTest::gtest
            GTest::gmock
            TBB::tbb
            fq_lib
            test_utils
        )
    else()
        target_link_libraries(${test_name} PRIVATE
            GTest::gtest_main
            GTest::gmock
            fq_lib
            test_utils
        )
    endif()

    # fq_lib 已传播 include/；src/ 用于访问内部实现头文件
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    add_test(NAME ${test_name} COMMAND ${test_name})

    if(ENABLE_TSAN)
        # TSan 插桩开销显著放大并发测试耗时，统一放宽 timeout
        math(EXPR _fq_test_timeout "${timeout} * 3")
    else()
        set(_fq_test_timeout ${timeout})
    endif()
    set_tests_properties(${test_name} PROPERTIES
        LABELS "${label}"
        TIMEOUT ${_fq_test_timeout}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # TSan 构建时为测试设置 suppressions 等选项（路径必须绝对，因为 ctest 会切换 CWD）
    if(ENABLE_TSAN)
        set_tests_properties(${test_name} PROPERTIES
            ENVIRONMENT
                "TSAN_OPTIONS=halt_on_error=0:second_deadlock_stack=1:suppressions=${CMAKE_SOURCE_DIR}/build-config/sanitizers/tsan.supp"
        )
    endif()
endfunction()

# --- 子目录 ---
add_subdirectory(utils)
add_subdirectory(unit)
add_subdirectory(integration)

set(FQTOOLS_PACKAGING_TEST_BINARY_DIR ${CMAKE_BINARY_DIR}/tests/packaging-build)
set(FQTOOLS_PACKAGING_TEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/tests/packaging-install)

add_test(
    NAME packaging_test
    COMMAND
        ${CMAKE_COMMAND}
        -DFQTOOLS_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DFQTOOLS_BINARY_DIR=${CMAKE_BINARY_DIR}
        -DFQTOOLS_CONSUMER_BINARY_DIR=${FQTOOLS_PACKAGING_TEST_BINARY_DIR}
        -DFQTOOLS_INSTALL_DIR=${FQTOOLS_PACKAGING_TEST_INSTALL_DIR}
        -DFQTOOLS_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -P ${CMAKE_SOURCE_DIR}/tests/packaging/verify_consumer.cmake
)
set_tests_properties(packaging_test PROPERTIES
    LABELS "integration"
    TIMEOUT 180
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

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

# TSan 构建时为 e2e 测试追加 suppressions 选项
if(ENABLE_TSAN)
    set_property(TEST e2e_shell_cli e2e_python_cli APPEND PROPERTY
        ENVIRONMENT
            "TSAN_OPTIONS=halt_on_error=0:second_deadlock_stack=1:suppressions=${CMAKE_SOURCE_DIR}/build-config/sanitizers/tsan.supp"
    )
endif()
