# tools/fuzz/CMakeLists.txt
#
# Fuzzing targets for FastQTools
#
# Build with: cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ ..
# Run with: ./fuzzers/fastq_parser_fuzzer corpus/

# Include fuzzing module
include(${CMAKE_SOURCE_DIR}/cmake/modules/fq_fuzzing.cmake)

if(NOT ENABLE_FUZZING)
    message(STATUS "Fuzzing disabled. Enable with -DENABLE_FUZZING=ON")
    return()
endif()

# =============================================================================
# FASTQ Parser Fuzzer
# =============================================================================

fq_add_fuzzer(fastq_parser_fuzzer
    SOURCES
        fastq_parser_fuzzer.cpp
    LIBRARIES
        fq_modern_io
        fq_common
)

# =============================================================================
# FASTQ Record Fuzzer
# =============================================================================

fq_add_fuzzer(fastq_record_fuzzer
    SOURCES
        fastq_record_fuzzer.cpp
    LIBRARIES
        fq_modern_io
        fq_common
)

# =============================================================================
# Corpus Directory
# =============================================================================

# Create corpus directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/corpus)

# Copy seed files to build directory
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/corpus)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/corpus
         DESTINATION ${CMAKE_BINARY_DIR}/fuzzers/)
endif()

message(STATUS "Fuzzing targets configured. Run fuzzers from: ${CMAKE_BINARY_DIR}/fuzzers/")
