import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run

required_conan_version = ">=2.2.0"

class PackageTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    win_bash = os.environ.get('MSYSTEM', None) is not None

    def requirements(self):
        self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self, build_folder="$TMP")

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "test_package-base")
            self.run(cmd, env="conanrun")
            cmd = os.path.join(self.cpp.build.bindir, "test_package-multi_user")
            if os.path.exists(cmd):
                self.run(cmd, env="conanrun")
            cmd = os.path.join(self.cpp.build.bindir, "test_package-spdlogging")
            if os.path.exists(cmd):
                self.run(cmd, env="conanrun")
