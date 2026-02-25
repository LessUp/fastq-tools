add_library(fq_error STATIC
    error.cpp
)

target_include_directories(fq_error
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# fq_common PUBLIC 传递 fmt::fmt，无需重复链接
# spdlog 仅在 error.cpp 中使用，设为 PRIVATE
target_link_libraries(fq_error
    PUBLIC
        fq_common
    PRIVATE
        spdlog::spdlog
)

target_compile_features(fq_error PUBLIC cxx_std_20)
