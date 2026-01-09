add_library(fq_modern_io STATIC
    fastq_reader.cpp
    fastq_writer.cpp
)

target_include_directories(fq_modern_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_modern_io
    PUBLIC
        ZLIB::ZLIB
        spdlog::spdlog
        fmt::fmt
)

 if(TARGET libdeflate::libdeflate)
     target_link_libraries(fq_modern_io PUBLIC libdeflate::libdeflate)
 elseif(TARGET libdeflate::libdeflate_static)
     target_link_libraries(fq_modern_io PUBLIC libdeflate::libdeflate_static)
 elseif(TARGET libdeflate::libdeflate_shared)
     target_link_libraries(fq_modern_io PUBLIC libdeflate::libdeflate_shared)
 endif()

target_compile_features(fq_modern_io PUBLIC cxx_std_20)
