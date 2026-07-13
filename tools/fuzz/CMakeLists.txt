# tools/fuzz/CMakeLists.txt
#
# 真实模糊测试目标 — 直接调用项目公共 API
#
# 构建：cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
# 运行：./build-fuzz/fuzzers/<name> tools/fuzz/corpus/ -max_len=4096
#
# 注：fq_fuzzing.cmake 与 fq_add_fuzzer 函数由根 CMakeLists.txt 引入，
#     此处无需重复 include。

if(NOT ENABLE_FUZZING)
    return()
endif()

# =============================================================================
# FASTQ Parser Fuzzer — fq::io::FastqReader::nextBatch
# 覆盖：指针算术、string_view 切片、\r\n 边界、remainder 跨批次、gzip sniff
# =============================================================================
fq_add_fuzzer(fastq_parser_fuzzer
    SOURCES
        fastq_parser_fuzzer.cpp
    LIBRARIES
        fq_modern_io
        fq_error
)

# =============================================================================
# FASTQ Mutator Fuzzer — QualityTrimmer/LengthTrimmer/AdapterTrimmer/PolyTailTrimmer
# 覆盖：substr 边界、AVX2 SIMD 越界读、adapter find 下溢
# =============================================================================
fq_add_fuzzer(fastq_mutator_fuzzer
    SOURCES
        fastq_mutator_fuzzer.cpp
    LIBRARIES
        fq_processing
)

# =============================================================================
# FASTQ Predicate Fuzzer — MinQuality/MinLength/MaxLength/MaxNRatio Predicate
# 覆盖：负值累加 clamp、空序列除零、极端阈值
# =============================================================================
fq_add_fuzzer(fastq_predicate_fuzzer
    SOURCES
        fastq_predicate_fuzzer.cpp
    LIBRARIES
        fq_processing
)

# =============================================================================
# FASTQ Statistics Fuzzer — FqStatisticWorker::calculateStats
# 覆盖：累加/取模/除法、ensureCapacity 扩容、kBaseLut 越界索引
# 需 PRIVATE include src/（fq_statistic_worker.h 是私有头）
# =============================================================================
fq_add_fuzzer(fastq_statistics_fuzzer
    SOURCES
        fastq_statistics_fuzzer.cpp
    LIBRARIES
        fq_statistics
)

# statistics fuzzer 需访问私有头 src/statistics/fq_statistic_worker.h
target_include_directories(fastq_statistics_fuzzer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# =============================================================================
# Corpus 目录
# =============================================================================

# 确保 corpus 目录存在
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/corpus)

# 将种子文件拷贝到构建目录
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/corpus)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/corpus
         DESTINATION ${CMAKE_BINARY_DIR}/fuzzers/)
endif()

message(STATUS "Fuzzing targets configured. Run fuzzers from: ${CMAKE_BINARY_DIR}/fuzzers/")
