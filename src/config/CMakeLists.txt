add_library(fq_config STATIC
    config.cpp
)

# fq_error PUBLIC 传递 fq_common 和 include 路径，无需重复声明
target_link_libraries(fq_config
    PUBLIC
        fq_error
)

target_compile_features(fq_config PUBLIC cxx_std_23)
