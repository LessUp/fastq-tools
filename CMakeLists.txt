cmake_minimum_required(VERSION 3.28)

project(FastQTools
    LANGUAGES CXX
    VERSION 4.1.0
    DESCRIPTION "C++23 FASTQ quality control tool with parallel pipeline core"
    HOMEPAGE_URL "https://github.com/open-genomics/fastq-tools"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Project Configuration ---
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Compiler Version Check ---
# Enforce minimum compiler versions required for C++23 support.
# GCC 13+ 和 Clang 17+ 才提供本仓库所需的 C++23 支持；更早版本会编译失败。
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.0")
        message(FATAL_ERROR "GCC version ${CMAKE_CXX_COMPILER_VERSION} is too old. "
                            "FastQTools requires GCC 13.0 or newer for C++23 support.")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
        message(FATAL_ERROR "Clang version ${CMAKE_CXX_COMPILER_VERSION} is too old. "
                            "FastQTools requires Clang 17.0 or newer for C++23 support.")
    endif()
else()
    message(WARNING "Compiler ${CMAKE_CXX_COMPILER_ID} is not officially supported. "
                    "Your build may fail due to lack of C++23 support.")
endif()


# --- Link-Time Optimization (LTO/IPO) ---
# Release 构建启用跨模块内联优化，显著提升小函数密集型项目的性能
option(ENABLE_LTO "Enable IPO/LTO for non-Debug builds when supported" ON)
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_OUTPUT)
if(ENABLE_LTO AND IPO_SUPPORTED AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "IPO/LTO is supported and enabled for non-Debug builds.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
else()
    if(NOT ENABLE_LTO)
        message(STATUS "IPO/LTO is disabled by ENABLE_LTO=OFF")
    elseif(NOT IPO_SUPPORTED)
        message(STATUS "IPO/LTO is not supported: ${IPO_OUTPUT}")
    endif()
endif()

# --- CPU baseline profile ---
# FQTOOLS_CPU_BASELINE controls the instruction set available to the compiler.
#   portable   : no -march flag (default; runs on any baseline x86-64/ARM CPU)
#   x86-64-v3  : -march=x86-64-v3 (AVX2+FMA+BMI; only on x86-64)
#   native     : -march=native (local benchmark only; not for release)
set(FQTOOLS_CPU_BASELINE "portable" CACHE STRING "CPU baseline profile: portable | x86-64-v3 | native")
set_property(CACHE FQTOOLS_CPU_BASELINE PROPERTY STRINGS portable x86-64-v3 native)

# Deprecated: ENABLE_NATIVE_ARCH maps to FQTOOLS_CPU_BASELINE=native for migration.
option(ENABLE_NATIVE_ARCH "Deprecated: use -DFQTOOLS_CPU_BASELINE=native instead" OFF)
if(ENABLE_NATIVE_ARCH)
    message(WARNING "ENABLE_NATIVE_ARCH is deprecated. Use -DFQTOOLS_CPU_BASELINE=native instead.")
    set(FQTOOLS_CPU_BASELINE "native" CACHE STRING "" FORCE)
endif()

# Validate and apply CPU baseline
if(NOT FQTOOLS_CPU_BASELINE STREQUAL "portable"
   AND NOT FQTOOLS_CPU_BASELINE STREQUAL "x86-64-v3"
   AND NOT FQTOOLS_CPU_BASELINE STREQUAL "native")
    message(FATAL_ERROR "Invalid FQTOOLS_CPU_BASELINE='${FQTOOLS_CPU_BASELINE}'. "
                         "Valid values: portable | x86-64-v3 | native")
endif()

message(STATUS "FastQTools CPU baseline: ${FQTOOLS_CPU_BASELINE}")

# Fast math: impairs numerical reproducibility, off by default
option(ENABLE_FAST_MATH "Enable -ffast-math (impairs numerical reproducibility)" OFF)

# Apply compiler flags (explicit STREQUAL to exclude AppleClang)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Apply CPU baseline only in Release (dev builds stay portable)
        if(FQTOOLS_CPU_BASELINE STREQUAL "native")
            add_compile_options(-march=native)
        elseif(FQTOOLS_CPU_BASELINE STREQUAL "x86-64-v3")
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|i386|i686")
                add_compile_options(-march=x86-64-v3)
            else()
                message(WARNING "FQTOOLS_CPU_BASELINE=x86-64-v3 ignored on ${CMAKE_SYSTEM_PROCESSOR}; using portable")
            endif()
        endif()
        # portable: no -march flag (baseline x86-64 / ARM default)
        add_compile_options(-O3 -ftree-vectorize -funroll-loops)
        if(ENABLE_FAST_MATH)
            add_compile_options(-ffast-math)
        endif()
        add_compile_options(-ffunction-sections -fdata-sections)
    endif()
endif()

# 链接器优化：移除未使用的段（ld64 不支持 --gc-sections，显式排除 AppleClang）
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_link_options(-Wl,--gc-sections)
endif()

# --- Sanitizer Support ---
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/fq_sanitizers.cmake)
fq_add_sanitizer_flags_global()

# --- Conan Integration ---
# This assumes you have run `conan install build-config/dependencies/ --build=missing`
# and the conan_toolchain.cmake file is in the build directory.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    else()
        message(STATUS "No Conan toolchain detected; attempting to use system packages.")
    endif()
endif()

# --- Optional build features ---
option(BUILD_BENCHMARKS "Build benchmark executables" OFF)

# --- Find Dependencies (handled by Conan) ---
find_package(cxxopts CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
# nlohmann_json 无条件查找：除 benchmark 模块外，benchmark 结果存储的单元测试
# （tests/unit/benchmark/test_result_storage.cpp）在默认构建中也直接消费
find_package(nlohmann_json CONFIG REQUIRED)
if(BUILD_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)
endif()

if(TARGET fmt::fmt-header-only AND NOT TARGET fmt::fmt)
    add_library(fmt::fmt ALIAS fmt::fmt-header-only)
endif()

# --- Optional clang-tidy integration ---
option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "Enabling clang-tidy during compilation: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(FATAL_ERROR "ENABLE_CLANG_TIDY=ON but clang-tidy was not found")
    endif()
endif()



# --- Code Coverage ---
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/fq_coverage.cmake)
if(ENABLE_COVERAGE)
    fq_add_coverage_flags_global()
endif()

# --- Add Subdirectories ---
# This now handles all library definitions and dependencies internally.
add_subdirectory(src)

if(BUILD_BENCHMARKS)
    add_subdirectory(tools/benchmark)
endif()

# --- Fuzzing Support ---
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/fq_fuzzing.cmake)
if(ENABLE_FUZZING)
    add_subdirectory(tools/fuzz)
endif()

# --- Testing ---
# include(CTest) 已定义 BUILD_TESTING option（默认 ON）并调用 enable_testing()，无需重复声明
include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
    add_subdirectory(tests)
endif()

# --- Installation ---
install(TARGETS FastQTools
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Runtime
)

# Install only the v4 public surface. Runtime pools and CLI-only helpers stay private.
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/fqtools/error
    ${CMAKE_SOURCE_DIR}/include/fqtools/io
    ${CMAKE_SOURCE_DIR}/include/fqtools/processing
    ${CMAKE_SOURCE_DIR}/include/fqtools/statistics
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fqtools
    COMPONENT Development
    FILES_MATCHING
    PATTERN "*.h"
)
install(FILES ${CMAKE_SOURCE_DIR}/include/fqtools/fq.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fqtools
    COMPONENT Development
)

# Export public library targets
install(TARGETS
    fq_runtime
    fq_error
    fq_modern_io
    fq_stat_core
    fq_processing
    fq_statistics
    EXPORT FastQToolsTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# fq_benchmark 为内部测量设施，不下发至公共 SDK


# --- Package Configuration ---
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/FastQTools)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FastQToolsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FastQToolsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FastQToolsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FastQToolsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/FastQToolsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(EXPORT FastQToolsTargets
    FILE FastQToolsTargets.cmake
    NAMESPACE FastQTools::
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT Development
)
