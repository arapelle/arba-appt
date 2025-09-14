import os, re

from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import load, copy, rmdir

required_conan_version = ">=2.2.0"

class ArbaApptRecipe(ConanFile):
    project_context_name = "arba"
    project_subject_name = "appt"
    name = f"{project_context_name}-{project_subject_name}"
    package_type = "library"

    # Optional metadata
    description = "A C++ library providing application classes embedding useful tools."
    url = f"https://github.com/arapelle/{name}"
    homepage = f"https://github.com/arapelle/{name}"
    topics = ("application", "app")
    license = "MIT"
    author = "Aymeric Pellé"

    # Components
    required_components = ["base"]
    optional_components = ["spdlogging"]

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    } | dict((x, [True, False]) for x in optional_components)
    default_options = {
        "shared": True,
        "fPIC": True,
    } | dict((x, True) for x in optional_components)

    # Build
    win_bash = os.environ.get('MSYSTEM', None) is not None
    no_copy_source = True

    # Sources
    exports_sources = "LICENSE.md", "CMakeLists.txt", "test/*", "external/*", "cmake/*", "component/*"

    # Other
    implements = ["auto_shared_fpic"]

    def set_version(self):
        cmakelist_content = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
        version_regex = r"""set_package_semantic_version\( *"?([0-9]+\.[0-9]+\.[0-9]+).*"""
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
        if self.options.spdlogging:
            self.requires("spdlog/[^1.8]", transitive_headers=True, transitive_libs=True)

    def build_requirements(self):
        self.test_requires("gtest/[^1.14]")

    def generate(self):
        deps = CMakeDeps(self)
        deps.check_components_exist = True
        deps.generate()
        tc = CMakeToolchain(self)
        upper_name = f"{self.project_context_name}_{self.project_subject_name}".upper()
        tc.variables[f"{upper_name}_LIBRARY_TYPE"] = "SHARED" if self.options.shared else "STATIC"
        # conan create . --build=missing -c:a "&:tools.build:skip_test=False"
        build_test = not self.conf.get("tools.build:skip_test", default=True)
        if build_test:
            tc.variables[f"BUILD_{upper_name}_TESTS"] = "TRUE"
            for comp in self.required_components:
                tc.variables[f"BUILD_{upper_name}_{comp.upper()}_TESTS"] = "TRUE"
        for comp in self.optional_components:
            comp_option = getattr(self.options, comp)
            if comp_option:
                if build_test:
                    tc.variables[f"BUILD_{upper_name}_{comp.upper()}_TESTS"] = "TRUE"
            else:
                tc.variables[f"BUILD_{upper_name}_{comp.upper()}"] = "FALSE"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=True):
            cmake.ctest(cli_args=["--progress", "--output-on-failure"])

    def package(self):
        copy(self, "LICENSE.md", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        build_type_postfix = "-d" if self.settings.build_type == "Debug" else ""
        # arba-appt::base
        comp_name = "base"
        comp_info = self.cpp_info.components[comp_name]
        comp_info.libs = [f"{comp_name}{build_type_postfix}"]
        comp_info.requires = ["arba-core::arba-core", "arba-stdx::arba-stdx", "arba-rsce::arba-rsce", "arba-evnt::arba-evnt"]
        # arba-appt::spdlog_logging
        if self.options.spdlogging:
            comp_name = "spdlogging"
            comp_info = self.cpp_info.components[comp_name]
            comp_info.libs = [f"{comp_name}{build_type_postfix}"]
            comp_info.requires = ["base", "spdlog::spdlog"]
