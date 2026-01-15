# cmake/modules/fq_sanitizers.cmake
#
# FastQTools Sanitizer Support Module
# Provides compiler sanitizer integration for memory, thread, and undefined behavior detection
#
# Usage:
#   include(cmake/modules/fq_sanitizers.cmake)
#   fq_add_sanitizer_flags(target_name)
#
# Options:
#   ENABLE_ASAN  - Enable AddressSanitizer (memory errors)
#   ENABLE_TSAN  - Enable ThreadSanitizer (data races)
#   ENABLE_UBSAN - Enable UndefinedBehaviorSanitizer
#   ENABLE_MSAN  - Enable MemorySanitizer (Clang only, uninitialized memory)

# =============================================================================
# Sanitizer Options
# =============================================================================

option(ENABLE_ASAN "Enable AddressSanitizer for memory error detection" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer for data race detection" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer (Clang only)" OFF)

# =============================================================================
# Sanitizer Validation
# =============================================================================

# Check for mutually exclusive sanitizers
set(_SANITIZER_COUNT 0)
if(ENABLE_ASAN)
    math(EXPR _SANITIZER_COUNT "${_SANITIZER_COUNT} + 1")
endif()
if(ENABLE_TSAN)
    math(EXPR _SANITIZER_COUNT "${_SANITIZER_COUNT} + 1")
endif()
if(ENABLE_MSAN)
    math(EXPR _SANITIZER_COUNT "${_SANITIZER_COUNT} + 1")
endif()

if(_SANITIZER_COUNT GREATER 1)
    message(FATAL_ERROR "Only one of ENABLE_ASAN, ENABLE_TSAN, or ENABLE_MSAN can be enabled at a time. "
                        "These sanitizers are mutually exclusive.")
endif()

# MSan requires Clang
if(ENABLE_MSAN AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "MemorySanitizer (MSAN) requires Clang compiler. "
                        "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# =============================================================================
# Sanitizer Flags
# =============================================================================

# Common flags for better error reporting
set(FQ_SANITIZER_COMMON_FLAGS "-fno-omit-frame-pointer -fno-optimize-sibling-calls")

# AddressSanitizer flags
set(FQ_ASAN_FLAGS "-fsanitize=address ${FQ_SANITIZER_COMMON_FLAGS}")

# ThreadSanitizer flags
set(FQ_TSAN_FLAGS "-fsanitize=thread ${FQ_SANITIZER_COMMON_FLAGS}")

# UndefinedBehaviorSanitizer flags
set(FQ_UBSAN_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all ${FQ_SANITIZER_COMMON_FLAGS}")

# MemorySanitizer flags (Clang only)
set(FQ_MSAN_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins=2 ${FQ_SANITIZER_COMMON_FLAGS}")

# =============================================================================
# Function: fq_add_sanitizer_flags
# =============================================================================

function(fq_add_sanitizer_flags target)
    if(NOT TARGET ${target})
        message(WARNING "fq_add_sanitizer_flags: Target '${target}' does not exist")
        return()
    endif()

    # AddressSanitizer
    if(ENABLE_ASAN)
        message(STATUS "Enabling AddressSanitizer for target: ${target}")
        target_compile_options(${target} PRIVATE 
            $<$<COMPILE_LANGUAGE:CXX>:${FQ_ASAN_FLAGS}>
        )
        target_link_options(${target} PRIVATE ${FQ_ASAN_FLAGS})
    endif()

    # ThreadSanitizer
    if(ENABLE_TSAN)
        message(STATUS "Enabling ThreadSanitizer for target: ${target}")
        target_compile_options(${target} PRIVATE 
            $<$<COMPILE_LANGUAGE:CXX>:${FQ_TSAN_FLAGS}>
        )
        target_link_options(${target} PRIVATE ${FQ_TSAN_FLAGS})
    endif()

    # UndefinedBehaviorSanitizer (can be combined with ASan or TSan)
    if(ENABLE_UBSAN)
        message(STATUS "Enabling UndefinedBehaviorSanitizer for target: ${target}")
        target_compile_options(${target} PRIVATE 
            $<$<COMPILE_LANGUAGE:CXX>:${FQ_UBSAN_FLAGS}>
        )
        target_link_options(${target} PRIVATE ${FQ_UBSAN_FLAGS})
    endif()

    # MemorySanitizer (Clang only)
    if(ENABLE_MSAN)
        message(STATUS "Enabling MemorySanitizer for target: ${target}")
        target_compile_options(${target} PRIVATE 
            $<$<COMPILE_LANGUAGE:CXX>:${FQ_MSAN_FLAGS}>
        )
        target_link_options(${target} PRIVATE ${FQ_MSAN_FLAGS})
    endif()
endfunction()

# =============================================================================
# Function: fq_add_sanitizer_flags_global
# =============================================================================

function(fq_add_sanitizer_flags_global)
    # AddressSanitizer
    if(ENABLE_ASAN)
        message(STATUS "Enabling AddressSanitizer globally")
        add_compile_options(${FQ_ASAN_FLAGS})
        add_link_options(${FQ_ASAN_FLAGS})
    endif()

    # ThreadSanitizer
    if(ENABLE_TSAN)
        message(STATUS "Enabling ThreadSanitizer globally")
        add_compile_options(${FQ_TSAN_FLAGS})
        add_link_options(${FQ_TSAN_FLAGS})
    endif()

    # UndefinedBehaviorSanitizer
    if(ENABLE_UBSAN)
        message(STATUS "Enabling UndefinedBehaviorSanitizer globally")
        add_compile_options(${FQ_UBSAN_FLAGS})
        add_link_options(${FQ_UBSAN_FLAGS})
    endif()

    # MemorySanitizer
    if(ENABLE_MSAN)
        message(STATUS "Enabling MemorySanitizer globally")
        add_compile_options(${FQ_MSAN_FLAGS})
        add_link_options(${FQ_MSAN_FLAGS})
    endif()
endfunction()

# =============================================================================
# Status Output
# =============================================================================

if(ENABLE_ASAN OR ENABLE_TSAN OR ENABLE_UBSAN OR ENABLE_MSAN)
    message(STATUS "=== Sanitizer Configuration ===")
    if(ENABLE_ASAN)
        message(STATUS "  AddressSanitizer: ON")
    endif()
    if(ENABLE_TSAN)
        message(STATUS "  ThreadSanitizer: ON")
    endif()
    if(ENABLE_UBSAN)
        message(STATUS "  UndefinedBehaviorSanitizer: ON")
    endif()
    if(ENABLE_MSAN)
        message(STATUS "  MemorySanitizer: ON")
    endif()
    message(STATUS "===============================")
endif()
