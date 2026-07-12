add_library(fq_cli STATIC
    filter_plan.cpp
    filter_plan.h
    commands/filter_command.cpp
    commands/filter_command.h
    commands/stat_command.cpp
    commands/stat_command.h
    commands/command_interface.h
    command_registry.cpp
    command_registry.h
    common_options.cpp
    common_options.h
    enum_parser.cpp
    enum_parser.h
)

target_link_libraries(fq_cli
    PUBLIC
        fq_lib
        cxxopts::cxxopts
)

target_include_directories(fq_cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 确保cxxopts头文件路径正确传递
target_compile_features(fq_cli PUBLIC cxx_std_23)

# --- 可执行入口 ---
# main.cpp 与 fq_cli 同目录，归属此处而非根 CMakeLists.txt
add_executable(FastQTools main.cpp)

# 产物固定输出到构建根，避免下沉后路径变为 build/src/cli/FastQTools
# 下游脚本/tests 默认期望 ${CMAKE_BINARY_DIR}/FastQTools
set_target_properties(FastQTools PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(FastQTools
    PRIVATE
        fq_cli
        fmt::fmt
)

target_compile_features(FastQTools PRIVATE cxx_std_23)
