# cmake/modules/fq_fuzzing.cmake
#
# FastQTools Fuzzing Support Module
# Provides libFuzzer integration for fuzz testing
#
# Usage:
#   include(cmake/modules/fq_fuzzing.cmake)
#   fq_add_fuzzer(fuzzer_name source_file.cpp)
#
# Options:
#   ENABLE_FUZZING - Enable fuzzing targets (requires Clang)
#
# Requirements:
#   - Clang compiler with libFuzzer support
#   - AddressSanitizer is automatically enabled with fuzzing

# =============================================================================
# Fuzzing Option
# =============================================================================

option(ENABLE_FUZZING "Enable fuzzing targets (requires Clang)" OFF)

# =============================================================================
# Fuzzing Validation
# =============================================================================

if(ENABLE_FUZZING)
    # Fuzzing requires Clang
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(WARNING "Fuzzing with libFuzzer requires Clang compiler. "
                        "Current compiler: ${CMAKE_CXX_COMPILER_ID}. "
                        "Fuzzing targets will be disabled.")
        set(ENABLE_FUZZING OFF CACHE BOOL "Enable fuzzing targets" FORCE)
    else()
        # Check Clang version (libFuzzer requires Clang 6.0+)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
            message(WARNING "libFuzzer requires Clang 6.0 or newer. "
                            "Current version: ${CMAKE_CXX_COMPILER_VERSION}. "
                            "Fuzzing targets will be disabled.")
            set(ENABLE_FUZZING OFF CACHE BOOL "Enable fuzzing targets" FORCE)
        endif()
    endif()
endif()

# =============================================================================
# Fuzzing Flags
# =============================================================================

if(ENABLE_FUZZING)
    # libFuzzer flags with AddressSanitizer
    set(FQ_FUZZER_COMPILE_FLAGS 
        "-fsanitize=fuzzer,address"
        "-fno-omit-frame-pointer"
        "-g"
    )
    
    set(FQ_FUZZER_LINK_FLAGS
        "-fsanitize=fuzzer,address"
    )
    
    # Optional: Add UBSan for better bug detection
    option(FUZZING_WITH_UBSAN "Enable UBSan with fuzzing" ON)
    if(FUZZING_WITH_UBSAN)
        list(APPEND FQ_FUZZER_COMPILE_FLAGS "-fsanitize=undefined")
        list(APPEND FQ_FUZZER_LINK_FLAGS "-fsanitize=undefined")
    endif()
endif()

# =============================================================================
# Function: fq_add_fuzzer
# =============================================================================

function(fq_add_fuzzer name)
    if(NOT ENABLE_FUZZING)
        return()
    endif()
    
    # Parse arguments
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs SOURCES LIBRARIES)
    cmake_parse_arguments(FUZZER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Handle legacy single-source call: fq_add_fuzzer(name source.cpp)
    if(NOT FUZZER_SOURCES AND FUZZER_UNPARSED_ARGUMENTS)
        set(FUZZER_SOURCES ${FUZZER_UNPARSED_ARGUMENTS})
    endif()
    
    if(NOT FUZZER_SOURCES)
        message(FATAL_ERROR "fq_add_fuzzer: No source files specified for fuzzer '${name}'")
    endif()
    
    message(STATUS "Adding fuzzer target: ${name}")
    
    # Create fuzzer executable
    add_executable(${name} ${FUZZER_SOURCES})
    
    # Add fuzzer compile flags
    target_compile_options(${name} PRIVATE ${FQ_FUZZER_COMPILE_FLAGS})
    
    # Add fuzzer link flags
    target_link_options(${name} PRIVATE ${FQ_FUZZER_LINK_FLAGS})
    
    # Link default libraries
    target_link_libraries(${name} PRIVATE
        fq_modern_io
        fq_common
        fq_error
    )
    
    # Link additional libraries if specified
    if(FUZZER_LIBRARIES)
        target_link_libraries(${name} PRIVATE ${FUZZER_LIBRARIES})
    endif()
    
    # Add include directories
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Set output directory
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/fuzzers"
    )
    
    # Add to fuzzer list
    set_property(GLOBAL APPEND PROPERTY FQ_FUZZER_TARGETS ${name})
endfunction()

# =============================================================================
# Custom Target: run-fuzzers
# =============================================================================

if(ENABLE_FUZZING)
    # Create corpus directory
    set(FQ_FUZZER_CORPUS_DIR "${CMAKE_SOURCE_DIR}/tools/fuzz/corpus")
    
    # Add custom target to list all fuzzers
    add_custom_target(list-fuzzers
        COMMAND ${CMAKE_COMMAND} -E echo "Available fuzzers:"
        COMMAND ${CMAKE_COMMAND} -E echo "  Run with: ./fuzzers/<name> corpus/"
        COMMENT "Listing available fuzzer targets"
    )
endif()

# =============================================================================
# Status Output
# =============================================================================

if(ENABLE_FUZZING)
    message(STATUS "=== Fuzzing Configuration ===")
    message(STATUS "  Fuzzing: ON")
    message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    set(_fq_fuzzing_sanitizers "address")
    if(FUZZING_WITH_UBSAN)
        string(APPEND _fq_fuzzing_sanitizers ", undefined")
    endif()
    message(STATUS "  Sanitizers: ${_fq_fuzzing_sanitizers}")
    message(STATUS "  Corpus directory: tools/fuzz/corpus/")
    message(STATUS "=============================")
endif()
