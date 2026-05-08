set(source_dir "$ENV{PWD}")
if(DEFINED FQTOOLS_SOURCE_DIR)
    set(source_dir "${FQTOOLS_SOURCE_DIR}")
endif()

if(NOT DEFINED FQTOOLS_BINARY_DIR)
    message(FATAL_ERROR "FQTOOLS_BINARY_DIR is required")
endif()
if(NOT DEFINED FQTOOLS_CONSUMER_BINARY_DIR)
    message(FATAL_ERROR "FQTOOLS_CONSUMER_BINARY_DIR is required")
endif()
if(NOT DEFINED FQTOOLS_INSTALL_DIR)
    message(FATAL_ERROR "FQTOOLS_INSTALL_DIR is required")
endif()
if(NOT DEFINED FQTOOLS_BUILD_TYPE OR FQTOOLS_BUILD_TYPE STREQUAL "")
    message(FATAL_ERROR "FQTOOLS_BUILD_TYPE is required")
endif()

file(REMOVE_RECURSE "${FQTOOLS_INSTALL_DIR}")
file(REMOVE_RECURSE "${FQTOOLS_CONSUMER_BINARY_DIR}")
file(MAKE_DIRECTORY "${FQTOOLS_INSTALL_DIR}")
file(MAKE_DIRECTORY "${FQTOOLS_CONSUMER_BINARY_DIR}")

execute_process(
    COMMAND ${CMAKE_COMMAND} --install "${FQTOOLS_BINARY_DIR}" --prefix "${FQTOOLS_INSTALL_DIR}"
    RESULT_VARIABLE install_result
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "Failed to install FastQTools for consumer verification")
endif()

set(consumer_source_dir "${source_dir}/tests/cmake_package_consumer")
set(deps_dir "${FQTOOLS_BINARY_DIR}")
set(fqtools_dir "${FQTOOLS_INSTALL_DIR}/lib/cmake/FastQTools")
set(toolchain_file "${FQTOOLS_BINARY_DIR}/conan_toolchain.cmake")

execute_process(
    COMMAND ${CMAKE_COMMAND}
        -S "${consumer_source_dir}"
        -B "${FQTOOLS_CONSUMER_BINARY_DIR}"
        -G Ninja
        "-DCMAKE_BUILD_TYPE=${FQTOOLS_BUILD_TYPE}"
        "-DCMAKE_TOOLCHAIN_FILE=${toolchain_file}"
        "-DFastQTools_DIR=${fqtools_dir}"
        "-DCMAKE_PREFIX_PATH=${deps_dir}"
    RESULT_VARIABLE configure_result
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "Failed to configure CMake package consumer")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build "${FQTOOLS_CONSUMER_BINARY_DIR}"
    RESULT_VARIABLE build_result
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "Failed to build CMake package consumer")
endif()

if(WIN32)
    set(consumer_exe "${FQTOOLS_CONSUMER_BINARY_DIR}/fqtools_consumer.exe")
else()
    set(consumer_exe "${FQTOOLS_CONSUMER_BINARY_DIR}/fqtools_consumer")
endif()

execute_process(
    COMMAND "${consumer_exe}"
    RESULT_VARIABLE run_result
    OUTPUT_VARIABLE run_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT run_result EQUAL 0)
    message(FATAL_ERROR "CMake package consumer executable failed")
endif()
if(NOT run_output STREQUAL "FastQTools")
    message(FATAL_ERROR "Unexpected consumer output: '${run_output}'")
endif()
