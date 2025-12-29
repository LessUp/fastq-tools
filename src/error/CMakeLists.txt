add_library(fq_error
    STATIC
        ${CMAKE_SOURCE_DIR}/include/fqtools/error/error.h
        error.cpp
)

target_include_directories(fq_error
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fq_error
    PUBLIC
        fq_common
        fmt::fmt
)

target_compile_features(fq_error PUBLIC cxx_std_20)

# 设置编译选项
target_compile_options(fq_error
    PRIVATE
        -Wall -Wextra -Wpedantic
)

# 设置 C++ 标准
set_property(TARGET fq_error PROPERTY CXX_STANDARD 20)
set_property(TARGET fq_error PROPERTY CXX_STANDARD_REQUIRED ON)
