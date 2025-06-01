import os, re

from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import load, copy, rmdir

required_conan_version = ">=2.2.0"

class ArbaApptRecipe(ConanFile):
    project_namespace = "arba"
    project_base_name = "appt"
    name = f"{project_namespace}-{project_base_name}"
    package_type = "library"

    # Optional metadata
    description = "A C++ library providing application classes embedding useful tools."
    url = "https://github.com/arapelle/arba-appt"
    homepage = "https://github.com/arapelle/arba-appt"
    topics = ("application", "app")
    license = "MIT"
    author = "Aymeric Pellé"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "test": [True, False],
        "use_spdlog": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "test": False,
        "use_spdlog": True
    }

    # Build
    win_bash = os.environ.get('MSYSTEM', None) is not None
    no_copy_source = True

    # Sources
    exports_sources = "LICENSE.md", "CMakeLists.txt", "test/*", "include/*", "src/*", "external/*", "cmake/*", "appt_feature/*"

    # Other
    implements = ["auto_shared_fpic"]

    def set_version(self):
        cmakelist_content = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
        version_regex = r"""set_project_semantic_version\( *"?([0-9]+\.[0-9]+\.[0-9]+).*"""
        self.version = re.search(version_regex, cmakelist_content).group(1)

    def layout(self):
        cmake_layout(self)

    def validate(self):
        check_min_cppstd(self, 20)
    
    def requirements(self):
        self.requires("arba-core/[^0.30]", transitive_headers=True, transitive_libs=True)
        self.requires("arba-stdx/[^0.3]", transitive_headers=True, transitive_libs=True)
        self.requires("arba-rsce/[^0.5]", transitive_headers=True, transitive_libs=True)
        self.requires("arba-evnt/[^0.7]", transitive_headers=True, transitive_libs=True)
        if self.options.use_spdlog:
            self.requires("spdlog/[^1.8]", transitive_headers=True, transitive_libs=True)

    def build_requirements(self):
        self.test_requires("gtest/[^1.14]")

    def generate(self):
        deps = CMakeDeps(self)
        deps.check_components_exist = True
        deps.generate()
        tc = CMakeToolchain(self)
        upper_name = f"{self.project_namespace}_{self.project_base_name}".upper()
        tc.variables[f"{upper_name}_LIBRARY_TYPE"] = "SHARED" if self.options.shared else "STATIC"
        if self.options.test:
            tc.variables[f"BUILD_{upper_name}_TESTS"] = "TRUE"
        if self.options.use_spdlog:
            tc.variables[f"{upper_name}_USE_SPDLOG"] = "TRUE"
        else:
            tc.variables[f"{upper_name}_USE_SPDLOG"] = "FALSE"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.test:
            cmake.ctest(cli_args=["--progress", "--output-on-failure"])

    def package(self):
        copy(self, "LICENSE.md", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        lib_type_postfix = "" if self.options.shared else "-static"
        build_type_postfix = "-d" if self.settings.build_type == "Debug" else ""
        # arba-appt::basic
        comp_name = "basic"
        basic_component = self.cpp_info.components[comp_name]
        basic_component.set_property("cmake_target_name", f"{self.name}::{comp_name}{lib_type_postfix}")
        basic_component.libs = [f"{self.name}-{comp_name}{lib_type_postfix}{build_type_postfix}"]
        basic_component.requires = ["arba-core::arba-core", "arba-stdx::arba-stdx", "arba-rsce::arba-rsce", "arba-evnt::arba-evnt"]
        if self.options.use_spdlog:
            basic_component.requires.append("spdlog::spdlog")
        # arba-appt::feature
        comp_name = "feature"
        feature_component = self.cpp_info.components[comp_name]
        feature_component.set_property("cmake_target_name", f"{self.name}::{comp_name}{lib_type_postfix}")
        feature_component.libs = [f"{self.name}-{comp_name}{lib_type_postfix}{build_type_postfix}"]
        # arba-appt::arba-appt
        self.cpp_info.set_property("cmake_target_name", f"{self.name}::{self.name}{lib_type_postfix}")
        self.cpp_info.libs = [f"{self.name}{lib_type_postfix}{build_type_postfix}"]
        self.cpp_info.requires = ["basic"]
