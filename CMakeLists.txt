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
        VERSION 0.10
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
    include/appt/user/user_manager.hpp
    include/appt/user/user_hash.hpp
    include/appt/user/user_set.hpp
    include/appt/user/user.hpp
    include/appt/application/decorator/multi_task.hpp
    include/appt/application/decorator/toolkit.hpp
    include/appt/application/decorator/multi_user.hpp
    include/appt/application/decorator/logging.hpp
    include/appt/application/application_logger.hpp
    include/appt/application/application.hpp
    include/appt/application/program.hpp
    include/appt/application/program_args.hpp
    include/appt/application/module/module_logger.hpp
    include/appt/application/module/decorator/loop.hpp
    include/appt/application/module/decorator/multi_user.hpp
    include/appt/application/module/decorator/logging.hpp
    include/appt/application/module/module.hpp
    include/appt/util/time_point_to_string.hpp
    include/appt/util/console_file_logger.hpp
    include/appt/util/integer_id_factory.hpp
    include/appt/util/duration.hpp
    include/appt/util/logging_macro.hpp
    include/appt/util/logger_helper.hpp
)

# Sources:
set(sources
    src/console_file_logger.cpp
    src/logger_helper.cpp
    src/program.cpp
    src/module.cpp
    src/user.cpp
    src/time_point_to_string.cpp
)

# Add C++ library
add_cpp_library(${PROJECT_NAME} ${PROJECT_NAME}_BUILD_SHARED_LIB ${PROJECT_NAME}_BUILD_STATIC_LIB
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    NAMESPACE arba::
    CXX_STANDARD 20
    INCLUDE_DIRECTORIES include
    INPUT_VERSION_HEADER "${CMAKE_CURRENT_LIST_DIR}/include.in/version.hpp"
    OUTPUT_VERSION_HEADER "version.hpp"
    HEADERS ${headers}
    SOURCES ${sources}
    BUILT_TARGETS project_targets
    )

# Install C++ library
install_cpp_library_targets(${PROJECT_NAME}
                            TARGETS ${project_targets}
                            INCLUDE_DIRECTORIES "include/${PROJECT_NAME}"
                            NAMESPACE arba::
                            )

# Link targets:
if(NOT TARGET arba::rsce)
    find_package(rsce 0.1 REQUIRED CONFIG)
endif()
if(NOT TARGET arba::evnt)
    find_package(evnt 0.2 REQUIRED CONFIG)
endif()
find_package(spdlog 1.8 REQUIRED CONFIG)
cpp_library_targets_link_libraries(${PROJECT_NAME} PUBLIC arba::rsce arba::evnt spdlog::spdlog)

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
