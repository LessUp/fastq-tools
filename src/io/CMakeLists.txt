add_library(fq_modern_io STATIC
    fastq_reader.cpp
    fastq_writer.cpp
)

target_include_directories(fq_modern_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ZLIB/fmt/fq_error 仅在 .cpp 中使用，不暴露给下游
target_link_libraries(fq_modern_io
    PRIVATE
        ZLIB::ZLIB
        fmt::fmt
        fq_error
)

target_compile_features(fq_modern_io PUBLIC cxx_std_23)

