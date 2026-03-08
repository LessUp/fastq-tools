# conanfile.py
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
import os

class FastQToolsConan(ConanFile):
    name = "fastqtools"
    version = "3.1.0"
    license = "MIT"
    author = "LessUp <jiashuai.mail@gmail.com>"
    url = "https://github.com/LessUp/FastQTools"
    description = "A modern toolkit for FASTQ file processing"
    topics = ("bioinformatics", "fastq", "genomics")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "config/*", "tests/*", "tools/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        # 避免编译 fmt 库本体在新版本 GCC/Clang 下触发 consteval 相关编译失败，改为纯头文件模式
        self.options["fmt"].header_only = True
        self.options["spdlog"].header_only = True
        self.options["spdlog"].use_std_fmt = False
        # onetbb 要求 hwloc 以 shared 模式构建
        self.options["hwloc"].shared = True

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("cxxopts/3.1.1")
        self.requires("spdlog/1.17.0")
        self.requires("fmt/12.1.0")
        self.requires("zlib-ng/2.3.2")
        self.requires("nlohmann_json/3.11.3")
        self.requires("onetbb/2022.3.0")
        self.requires("libdeflate/1.25", override=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        # Append Clang-specific flags ONLY when using libc++ to avoid conflicts on libstdc++11
        try:
            libcxx = str(self.settings.compiler.libcxx)
        except Exception:
            libcxx = ""
        if str(self.settings.compiler) == "clang" and libcxx == "libc++":
            toolchain_path = os.path.join(self.generators_folder, "conan_toolchain.cmake")
            with open(toolchain_path, "a") as f:
                f.write("""
# Fix Clang-specific compiler flags when using libc++
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
""")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["fq_cli", "fq_processing", "fq_statistics", "fq_modern_io", "fq_config", "fq_error", "fq_common"]
        self.cpp_info.set_property("cmake_file_name", "FastQTools")
        self.cpp_info.set_property("cmake_target_name", "FastQTools::FastQTools")
