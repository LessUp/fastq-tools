# cmake/modules/fq_coverage.cmake
#
# FastQTools Code Coverage Module
# Provides code coverage instrumentation and reporting support
#
# Usage:
#   include(cmake/modules/fq_coverage.cmake)
#   fq_add_coverage_flags(target_name)
#
# Options:
#   ENABLE_COVERAGE - Enable code coverage instrumentation
#
# Requirements:
#   - GCC or Clang compiler
#   - lcov and genhtml for report generation
#   - Debug build type recommended for accurate line coverage

# =============================================================================
# Coverage Option
# =============================================================================

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

# =============================================================================
# Coverage Validation
# =============================================================================

if(ENABLE_COVERAGE)
    # Coverage requires Debug build for accurate results
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Coverage")
        message(WARNING "Code coverage is most accurate with Debug build type. "
                        "Current build type: ${CMAKE_BUILD_TYPE}")
    endif()
    
    # Check compiler support
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Code coverage requires GCC or Clang compiler. "
                            "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

# =============================================================================
# Coverage Flags
# =============================================================================

# GCC/Clang coverage flags
set(FQ_COVERAGE_COMPILE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
set(FQ_COVERAGE_LINK_FLAGS "--coverage")

# Additional flags for better coverage data
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang-specific: use source-based coverage for more accurate data
    set(FQ_COVERAGE_COMPILE_FLAGS "${FQ_COVERAGE_COMPILE_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    set(FQ_COVERAGE_LINK_FLAGS "${FQ_COVERAGE_LINK_FLAGS} -fprofile-instr-generate")
endif()

# =============================================================================
# Function: fq_add_coverage_flags
# =============================================================================

function(fq_add_coverage_flags target)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()
    
    if(NOT TARGET ${target})
        message(WARNING "fq_add_coverage_flags: Target '${target}' does not exist")
        return()
    endif()
    
    message(STATUS "Enabling code coverage for target: ${target}")
    
    # Add compile flags
    target_compile_options(${target} PRIVATE 
        $<$<COMPILE_LANGUAGE:CXX>:--coverage -fprofile-arcs -ftest-coverage>
    )
    
    # Add link flags
    target_link_options(${target} PRIVATE --coverage)
    
    # Disable optimizations for accurate coverage
    target_compile_options(${target} PRIVATE -O0 -g)
endfunction()

# =============================================================================
# Function: fq_add_coverage_flags_global
# =============================================================================

function(fq_add_coverage_flags_global)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()
    
    message(STATUS "Enabling code coverage globally")
    
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage -O0 -g)
    add_link_options(--coverage)
endfunction()

# =============================================================================
# Custom Target: coverage-report
# =============================================================================

if(ENABLE_COVERAGE)
    # Find coverage tools
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Found lcov: ${LCOV_PATH}")
        message(STATUS "Found genhtml: ${GENHTML_PATH}")
        
        # Add custom target for generating coverage report
        add_custom_target(coverage-report
            COMMAND ${LCOV_PATH} --capture 
                    --directory ${CMAKE_BINARY_DIR} 
                    --output-file ${CMAKE_BINARY_DIR}/coverage.info
                    --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage.info
                    '/usr/*'
                    '*/tests/*'
                    '*/third_party/*'
                    '*/build*/*'
                    --output-file ${CMAKE_BINARY_DIR}/coverage.filtered.info
                    --ignore-errors unused
            COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/coverage.filtered.info
                    --output-directory ${CMAKE_BINARY_DIR}/coverage_report
                    --title "FastQTools Coverage Report"
                    --legend
                    --show-details
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating code coverage report..."
        )
        
        # Add custom target for coverage summary
        add_custom_target(coverage-summary
            COMMAND ${LCOV_PATH} --summary ${CMAKE_BINARY_DIR}/coverage.filtered.info
            DEPENDS coverage-report
            COMMENT "Coverage summary:"
        )
        
        # Add custom target to clean coverage data
        add_custom_target(coverage-clean
            COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -delete
            COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcno" -delete
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage_report
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/coverage.info
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/coverage.filtered.info
            COMMENT "Cleaning coverage data..."
        )
    else()
        message(WARNING "lcov or genhtml not found. Coverage report generation disabled.")
        message(STATUS "Install with: sudo apt-get install lcov")
    endif()
endif()

# =============================================================================
# Status Output
# =============================================================================

if(ENABLE_COVERAGE)
    message(STATUS "=== Coverage Configuration ===")
    message(STATUS "  Coverage: ON")
    message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "  Report generation: Available")
        message(STATUS "  Run 'make coverage-report' after tests to generate report")
    else()
        message(STATUS "  Report generation: Unavailable (install lcov)")
    endif()
    message(STATUS "==============================")
endif()
