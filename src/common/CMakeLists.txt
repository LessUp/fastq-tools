add_library(fq_common STATIC
    common.cpp
)

target_include_directories(fq_common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_common
    PUBLIC
        fmt::fmt
)

target_compile_features(fq_common PUBLIC cxx_std_20)
