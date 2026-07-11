add_library(fq_error STATIC
    error.cpp
)

# fq_common PUBLIC 传递 fmt::fmt 和 include 路径，无需重复声明
target_link_libraries(fq_error
    PUBLIC
        fq_common
)

target_compile_features(fq_error PUBLIC cxx_std_23)
