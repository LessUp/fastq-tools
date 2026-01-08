# Add components directly
add_subdirectory(common)
add_subdirectory(error)
add_subdirectory(config)

add_subdirectory(processing)
add_subdirectory(statistics)
add_subdirectory(io)
add_subdirectory(cli)

# Create a new main library that will aggregate all components (new modules and legacy code)
add_library(fq_lib INTERFACE)

target_link_libraries(fq_lib 
INTERFACE 
fq_config  # This transitively links fq_error and fq_common
fq_common
fq_error

fq_modern_io
fq_processing
fq_statistics
fmt::fmt  # Add explicit fmt linking
)

target_include_directories(fq_lib
INTERFACE
${CMAKE_SOURCE_DIR}/include
)
