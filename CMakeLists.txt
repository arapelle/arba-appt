cmake_minimum_required(VERSION 3.18)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include(CMakePrintHelpers)
include(cmtk/CppLibraryProject)

#-----
# PROJECT

check_cmake_binary_dir()
set_build_type_if_undefined()

#-----
# C++ PROJECT

project(appt
        VERSION 0.1.0
        DESCRIPTION "A C++ library providing application objects embedding useful tools."
        HOMEPAGE_URL "https://github.com/arapelle/appt"
        LANGUAGES CXX)

message(STATUS "BUILD   : ${CMAKE_BUILD_TYPE}")
message(STATUS "CPPCOMP : ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "PROJECT : ${PROJECT_NAME} ${PROJECT_VERSION}")

#-----
include(CTest)

# Project options
library_build_options(${PROJECT_NAME} STATIC SHARED EXAMPLE TEST)

# Headers:
set(headers
    include/appt/application.hpp
    include/appt/module.hpp
    include/appt/multi_task_application.hpp
    include/appt/multi_user_application.hpp
    include/appt/multi_user_module.hpp
    include/appt/program_args.hpp
    include/appt/user.hpp
    include/appt/user_manager.hpp
    include/appt/user_set.hpp
    include/appt/user_hash.hpp
    include/appt/util/integer_id_factory.hpp
    include/appt/util/duration.hpp
)

# Sources:
set(sources
    src/application.cpp
    src/module.cpp
    src/user.cpp
)

# Add C++ library
add_cpp_library(${PROJECT_NAME} ${PROJECT_NAME}_BUILD_SHARED_LIB ${PROJECT_NAME}_BUILD_STATIC_LIB
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    CXX_STANDARD 20
    INCLUDE_DIRECTORIES include
    OUTPUT_VERSION_HEADER "version.hpp"
    HEADERS ${headers}
    SOURCES ${sources}
    BUILT_TARGETS project_targets
    )

# Install C++ library
install_cpp_library_targets(${PROJECT_NAME}
                            TARGETS ${project_targets}
                            INCLUDE_DIRECTORIES "include/${PROJECT_NAME}"
                            )

# Link targets:
if(NOT TARGET arba::rsce)
    find_package(rsce 0.1 REQUIRED CONFIG)
endif()
if(NOT TARGET arba::evnt)
    find_package(evnt 0.1 REQUIRED CONFIG)
endif()
cpp_library_targets_link_libraries(${PROJECT_NAME} PUBLIC arba::rsce arba::evnt)

# Install package
install_package(${PROJECT_NAME}
                INPUT_PACKAGE_CONFIG_FILE CMake-package-config.cmake.in
                EXPORT_NAMES ${PROJECT_NAME})

if(${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(${PROJECT_NAME}_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(test)
endif()

#-----
