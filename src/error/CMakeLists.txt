add_library(fq_error STATIC
    error.cpp
)

# fq_common PUBLIC 传递 fmt::fmt 和 include 路径，无需重复声明
# spdlog 仅在 error.cpp 中使用，设为 PRIVATE
target_link_libraries(fq_error
    PUBLIC
        fq_common
    PRIVATE
        spdlog::spdlog
)

target_compile_features(fq_error PUBLIC cxx_std_23)
