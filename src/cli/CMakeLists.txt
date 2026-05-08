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
