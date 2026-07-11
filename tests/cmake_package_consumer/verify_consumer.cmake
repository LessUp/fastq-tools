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
set(consumer_link_flags "")

set(cache_file "${FQTOOLS_BINARY_DIR}/CMakeCache.txt")
if(EXISTS "${cache_file}")
    file(STRINGS "${cache_file}" asan_enabled REGEX "^ENABLE_ASAN:BOOL=ON$")
    file(STRINGS "${cache_file}" tsan_enabled REGEX "^ENABLE_TSAN:BOOL=ON$")
    file(STRINGS "${cache_file}" ubsan_enabled REGEX "^ENABLE_UBSAN:BOOL=ON$")
    file(STRINGS "${cache_file}" msan_enabled REGEX "^ENABLE_MSAN:BOOL=ON$")
    file(STRINGS "${cache_file}" lto_enabled REGEX "^ENABLE_LTO:BOOL=ON$")

    if(asan_enabled)
        string(APPEND consumer_link_flags " -fsanitize=address")
    endif()
    if(tsan_enabled)
        string(APPEND consumer_link_flags " -fsanitize=thread")
    endif()
    if(ubsan_enabled)
        string(APPEND consumer_link_flags " -fsanitize=undefined -fno-sanitize-recover=all")
    endif()
    if(msan_enabled)
        string(APPEND consumer_link_flags " -fsanitize=memory -fsanitize-memory-track-origins=2")
    endif()
    # 主项目启用 LTO 时，安装的 .a 是 ThinLTO bitcode，
    # consumer 也需要启用 LTO 才能链接
    if(lto_enabled)
        string(APPEND consumer_link_flags " -flto=thin")
    endif()

    string(STRIP "${consumer_link_flags}" consumer_link_flags)
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND}
        -S "${consumer_source_dir}"
        -B "${FQTOOLS_CONSUMER_BINARY_DIR}"
        -G Ninja
        "-DCMAKE_BUILD_TYPE=${FQTOOLS_BUILD_TYPE}"
        "-DCMAKE_TOOLCHAIN_FILE=${toolchain_file}"
        "-DFastQTools_DIR=${fqtools_dir}"
        "-DCMAKE_PREFIX_PATH=${deps_dir}"
        "-DCMAKE_EXE_LINKER_FLAGS=${consumer_link_flags}"
        "-DCMAKE_SHARED_LINKER_FLAGS=${consumer_link_flags}"
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
