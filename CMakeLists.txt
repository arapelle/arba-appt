cmake_minimum_required(VERSION 3.26)

string(TIMESTAMP CONFIGURE_DATETIME "%Y%m%d-%H%M%S")

# EXTERNAL DEPENDENCIES

add_subdirectory(external)

# CMAKE INCLUDES

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/external/cmake/)
include(CMakePrintHelpers)
include(cmtk/CppLibraryProject)

# CONFIGURATION

disable_in_source_build()
set_build_type_ifndef()

# C++ PROJECT

set_package_name(CONTEXT_NAME "arba" SUBJECT_NAME "appt")
set_package_semantic_version("0.18.0" PRE_RELEASE "dev" BUILD_METADATA "${CONFIGURE_DATETIME}")

project(${PACKAGE_NAME}
        VERSION ${PACKAGE_VERSION}
        DESCRIPTION "A C++ library providing application classes embedding useful tools."
        HOMEPAGE_URL "https://github.com/arapelle/arba-appt"
        LANGUAGES CXX)
message(STATUS "PROJECT : ${PACKAGE_NAME} ${PACKAGE_SEMANTIC_VERSION} ${CMAKE_BUILD_TYPE}")

cxx_standard_option(${PACKAGE_UPPER_VAR_NAME}_CXX_STANDARD MIN 20 MAX 26)
shared_or_static_option(${PACKAGE_UPPER_VAR_NAME}_LIBRARY_TYPE "SHARED")

## Resolve dependencies:
find_package(arba-core 0.30.0 REQUIRED CONFIG)
find_package(arba-stdx 0.3.0 REQUIRED CONFIG)
find_package(arba-rsce 0.5.0 REQUIRED CONFIG)
find_package(arba-evnt 0.7.0 REQUIRED CONFIG)
build_component_option(COMPONENT_NAME spdlogging)
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_SPDLOGGING})
    find_package(spdlog 1.8.0 REQUIRED CONFIG)
endif()

# Add main target:
add_library(${PACKAGE_NAME} INTERFACE)
add_test_subdirectory_if_built(test NAME ${PACKAGE_NAME})
build_examples_option(NAME ${PACKAGE_NAME})

# Add components:
add_subdirectory(component/base)
add_component_subdirectory_if_built(component/spdlogging COMPONENT_NAME spdlogging)

# Install main target:
install_cpp_targets(TARGETS ${PACKAGE_NAME} EXPORT ${PACKAGE_NAME}-targets NAMESPACE "${PACKAGE_NAME}::")

## Install package:
install_package(${PACKAGE_NAME} INPUT_PACKAGE_CONFIG_FILE cmake/config/package-config.cmake.in)
install_uninstall_script(${PACKAGE_NAME})
