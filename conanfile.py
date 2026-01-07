# conanfile.py
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

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
    exports_sources = "CMakeLists.txt", "src/*", "cmake/*", "third_party/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("cxxopts/3.1.1")
        self.requires("spdlog/1.15.0")
        self.requires("fmt/11.0.2")
        self.requires("zlib/1.3")
        self.requires("bzip2/1.0.8")
        self.requires("xz_utils/5.4.5")
        self.requires("nlohmann_json/3.11.2")
        self.requires("onetbb/2021.10.0")
        self.requires("libdeflate/1.19", override=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["fq_lib", "fq_cli"]
        self.cpp_info.set_property("cmake_file_name", "FastQTools")
        self.cpp_info.set_property("cmake_target_name", "FastQTools::FastQTools")
