add_library(fq_config STATIC
    config.cpp
    ${CMAKE_SOURCE_DIR}/include/fqtools/config/config.h
)

target_include_directories(fq_config
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_config
    PUBLIC
        fq_error
)

target_compile_features(fq_config PUBLIC cxx_std_20)
