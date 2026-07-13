message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(cxxopts)
find_package(zlib-ng)
find_package(fmt)
find_package(nlohmann_json)
find_package(TBB)
find_package(benchmark)
find_package(GTest)

set(CONANDEPS_LEGACY  cxxopts::cxxopts  zlib-ng::zlib-ng  fmt::fmt-header-only  nlohmann_json::nlohmann_json  onetbb::onetbb  benchmark::benchmark_main  gtest::gtest )