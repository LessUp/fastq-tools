enable_testing()

find_package(GTest REQUIRED)

add_subdirectory(utils)
add_subdirectory(unit)
add_subdirectory(integration)
