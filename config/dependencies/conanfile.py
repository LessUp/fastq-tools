from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.cmake import cmake_layout
import os

class FastQTools(ConanFile):
    name = "fastqtools"
    version = "3.1.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    
    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "app/*", "cmake/*"

    def configure(self):
        # 避免编译 fmt 库本体在新版本 GCC/Clang 下触发 consteval 相关编译失败，改为纯头文件模式
        self.options["fmt"].header_only = True
        self.options["spdlog"].header_only = True
        self.options["spdlog"].use_std_fmt = False

    def requirements(self):
        """
        Declare all dependencies for the project.
        """
        self.requires("cxxopts/3.1.1")
        self.requires("spdlog/1.17.0")
        self.requires("zlib-ng/2.3.2")
        self.requires("fmt/12.1.0")
        self.requires("nlohmann_json/3.11.3")
        # Intel's Threading Building Blocks for high-level parallelism
        self.requires("onetbb/2022.3.0")
        self.requires("libdeflate/1.25")
        self.requires("benchmark/1.8.3")

    def build_requirements(self):
        """
        Dependencies required only for building the project, like testing frameworks.
        """
        self.tool_requires("cmake/[>=3.20]")
        if self.options.get_safe("build_testing", True):
            self.requires("gtest/1.14.0")

    def generate(self):
        """
        Generate build system files.
        """
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        
        # Generate CMake toolchain file
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
        """
        Build the project using CMake.
        """
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        """
        Package the project artifacts.
        """
        cmake = CMake(self)
        cmake.install()
