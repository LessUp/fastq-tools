add_library(fq_error STATIC
    error.cpp
)

target_include_directories(fq_error
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_error
    PUBLIC
        fmt::fmt
)

target_compile_features(fq_error PUBLIC cxx_std_23)
