from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
import os

class FastQTools(ConanFile):
    name = "fastqtools"
    version = "4.0.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "build_testing": [True, False],
        "build_benchmarks": [True, False],
    }
    default_options = {
        "build_testing": True,
        "build_benchmarks": False,
    }
    
    # Sources are located in the same place as this recipe, copy them to the recipe
    # 注意：tests 的 TSAN_OPTIONS 引用 ${CMAKE_SOURCE_DIR}/build-config/sanitizers/tsan.supp，
    # 该文件位于 recipe 目录之外，而 Conan 不允许 exports_sources 引用 recipe 目录之外的文件，
    # 因此 conan create 导出构建场景不支持 TSan 测试；本地 conan install + 源码内构建不受影响。
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "tests/*", "tools/*"

    def configure(self):
        # 避免编译 fmt 库本体在新版本 GCC/Clang 下触发 consteval 相关编译失败，改为纯头文件模式
        self.options["fmt"].header_only = True
        # onetbb 要求 hwloc 以 shared 模式构建
        self.options["hwloc"].shared = True

    def requirements(self):
        """
        Declare all dependencies for the project.
        """
        self.requires("cxxopts/3.3.1")
        self.requires("zlib-ng/2.3.3")
        self.requires("fmt/12.1.0")
        # Intel's Threading Building Blocks for high-level parallelism
        self.requires("onetbb/2022.3.0")
        if self.options.build_benchmarks:
            self.requires("benchmark/1.9.5")
            self.requires("nlohmann_json/3.12.0")

    def build_requirements(self):
        """
        Dependencies required only for building the project, like testing frameworks.
        """
        # 与 cmake_minimum_required(VERSION 3.28) 保持一致
        self.tool_requires("cmake/[>=3.28]")
        if self.options.build_testing:
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
        cmake.configure(variables={
            "BUILD_TESTING": self.options.build_testing,
            "BUILD_BENCHMARKS": self.options.build_benchmarks,
        })
        cmake.build()

    def package(self):
        """
        Package the project artifacts.
        """
        cmake = CMake(self)
        cmake.install()
