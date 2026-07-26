cmake_minimum_required(VERSION 3.28)

project(FastQTools
    LANGUAGES CXX
    VERSION 4.0.0
    DESCRIPTION "C++23 FASTQ quality control tool with parallel pipeline core"
    HOMEPAGE_URL "https://github.com/LessUp/fastq-tools"
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

# --- Compiler-specific options ---
# 架构扩展：默认 x86-64-v3（AVX2+，可移植），可选 native 用于本地极致性能
option(ENABLE_NATIVE_ARCH "Use -march=native for local build (not portable)" OFF)
# 快速浮点：允许重排浮点运算，提升向量化但牺牲精度与可复现性
option(ENABLE_FAST_MATH "Enable -ffast-math (impairs numerical reproducibility)" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        # Release build optimizations
        if(ENABLE_NATIVE_ARCH)
            add_compile_options(-march=native)
        else()
            # 按架构选择可移植基线：x86-64-v3（AVX2+FMA+BMI）或 armv8-a
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|i386|i686")
                add_compile_options(-march=x86-64-v3)
            elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|arm")
                add_compile_options(-march=armv8-a)
            endif()
        endif()
        # 显式启用向量化和循环展开（-O3 默认包含，但显式声明确保一致性）
        add_compile_options(-O3 -ftree-vectorize -funroll-loops)
        # -ffast-math 影响统计模块数值可复现性，默认关闭
        if(ENABLE_FAST_MATH)
            add_compile_options(-ffast-math)
        endif()
        # 使用函数段和数据段，配合链接器 --gc-sections 消除未使用代码
        # 仅在非 Debug 构建启用，避免影响调试体验
        add_compile_options(-ffunction-sections -fdata-sections)
    endif()
endif()

# 链接器优化：移除未使用的段
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
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
if(BUILD_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
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
include(CTest)
option(BUILD_TESTING "Build tests" ON)
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
    PATTERN "fastq_batch_pool.h" EXCLUDE
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
