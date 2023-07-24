cmake_minimum_required(VERSION 3.26)

# CMAKE INCLUDES

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/external/cmake)
include(CMakePrintHelpers)
include(cmtk/CppLibraryProject)

# CONFIGURATION

disable_in_source_build()
set_build_type_ifndef()

# C++ PROJECT

set(PROJECT_NAME "arba-appt")
set(project_namespace "arba")
set(project_base_name "appt")
set(PROJECT_VERSION 0.12.0)
project(${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "A C++ library providing application objects embedding useful tools."
        HOMEPAGE_URL "https://github.com/arapelle/appt"
        LANGUAGES CXX)
message(STATUS "PROJECT : ${PROJECT_NAME} ${PROJECT_VERSION} ${CMAKE_BUILD_TYPE}")
include(CTest)

# C++ LIBRARY

## Generated/Configured headers:
generate_version_macro_header(generated_version_header
                              ${project_namespace}_${project_base_name}
                              ${project_namespace}/${project_base_name}/version.hpp)

## Headers:
set(headers
    include/arba/appt/user/user_manager.hpp
    include/arba/appt/user/user_hash.hpp
    include/arba/appt/user/user_set.hpp
    include/arba/appt/user/user.hpp
    include/arba/appt/application/decorator/multi_task.hpp
    include/arba/appt/application/decorator/toolkit.hpp
    include/arba/appt/application/decorator/multi_user.hpp
    include/arba/appt/application/decorator/logging.hpp
    include/arba/appt/application/application_logger.hpp
    include/arba/appt/application/application.hpp
    include/arba/appt/application/program.hpp
    include/arba/appt/application/program_args.hpp
    include/arba/appt/application/module/module_logger.hpp
    include/arba/appt/application/module/decorator/loop.hpp
    include/arba/appt/application/module/decorator/multi_user.hpp
    include/arba/appt/application/module/decorator/logging.hpp
    include/arba/appt/application/module/module.hpp
    include/arba/appt/util/time_point_to_string.hpp
    include/arba/appt/util/console_file_logger.hpp
    include/arba/appt/util/integer_id_factory.hpp
    include/arba/appt/util/duration.hpp
    include/arba/appt/util/logging_macro.hpp
    include/arba/appt/util/logger_helper.hpp
)

## Sources:
set(sources
    src/console_file_logger.cpp
    src/logger_helper.cpp
    src/program.cpp
    src/module.cpp
    src/user.cpp
    src/time_point_to_string.cpp
)

## Add C++ library:
add_cpp_library(
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    BUILD_SHARED BUILD_arba-arba
    BUILD_STATIC BUILD_arba-arba-static
    HEADERS ${headers} ${generated_version_header}
    SOURCES ${sources}
    CXX_STANDARD 20
)

## Link C++ targets:
if(NOT TARGET arba-rsce)
    find_package(arba-rsce 0.2.0 REQUIRED CONFIG)
endif()
if(NOT TARGET arba-evnt)
    find_package(arba-evnt 0.3.0 REQUIRED CONFIG)
endif()
find_package(spdlog 1.8 REQUIRED CONFIG)
cpp_library_link_libraries(
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    PUBLIC arba-rsce arba-evnt spdlog::spdlog
)

## Install C++ library:
install_cpp_library(
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    EXPORT ${PROJECT_NAME}-targets
)

## Install project package
install_library_package(${PROJECT_NAME} INPUT_PACKAGE_CONFIG_FILE cmake/config/package-config.cmake.in)
install_uninstall_script(${PROJECT_NAME} FILENAME uninstall.cmake)

## Add examples:
option_or_set_ifdef(BUILD_${PROJECT_NAME}_EXAMPLES "Build ${PROJECT_NAME} examples or not" OFF BUILD_arba-arba_EXAMPLES)
if(BUILD_${PROJECT_NAME}_EXAMPLES)
    add_subdirectory(example)
endif()

## Add tests:
option_or_set_ifdef(BUILD_${PROJECT_NAME}_TESTS "Build ${PROJECT_NAME} tests or not" OFF BUILD_arba-arba_TESTS)
option(BUILD_${PROJECT_NAME}_TESTS "Build ${PROJECT_NAME} tests or not" OFF)
if(BUILD_${PROJECT_NAME}_TESTS AND BUILD_TESTING)
    add_subdirectory(test)
endif()
