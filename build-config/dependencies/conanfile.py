from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.cmake import cmake_layout
import os

class FastQTools(ConanFile):
    name = "fastqtools"
    version = "3.2.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"with_taskflow": [True, False]}
    default_options = {"with_taskflow": False}
    
    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "config/*", "tests/*", "tools/*"

    def configure(self):
        # 避免编译 fmt 库本体在新版本 GCC/Clang 下触发 consteval 相关编译失败，改为纯头文件模式
        self.options["fmt"].header_only = True
        # onetbb 要求 hwloc 以 shared 模式构建
        self.options["hwloc"].shared = True

    def requirements(self):
        """
        Declare all dependencies for the project.
        """
        self.requires("cxxopts/3.1.1")
        self.requires("zlib-ng/2.3.3")
        self.requires("fmt/12.1.0")
        self.requires("nlohmann_json/3.12.0")
        # Intel's Threading Building Blocks for high-level parallelism
        self.requires("onetbb/2022.3.0")
        if self.options.with_taskflow:
            self.requires("taskflow/4.0.0")
        self.requires("benchmark/1.9.5")

    def build_requirements(self):
        """
        Dependencies required only for building the project, like testing frameworks.
        """
        self.tool_requires("cmake/[>=3.20]")
        if self.options.get_safe("build_testing", True):
            self.requires("gtest/1.17.0")

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
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
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
