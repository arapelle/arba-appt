cmake_minimum_required(VERSION 3.26)

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

set_project_name(NAMESPACE "arba" FEATURE_NAME "appt")
string(TIMESTAMP configure_datetime "%Y%m%d-%H%M%S")
set_project_semantic_version("0.18.0" PRE_RELEASE "dev" BUILD_METADATA "${configure_datetime}")

project(${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "A C++ library providing application classes embedding useful tools."
        HOMEPAGE_URL "https://github.com/arapelle/arba-appt"
        LANGUAGES CXX)
message(STATUS "PROJECT : ${PROJECT_NAME} ${PROJECT_SEMANTIC_VERSION} ${CMAKE_BUILD_TYPE}")

option(ARBA_APPT_USE_SPDLOG "Use spdlog to provide logging features." ON)

# C++ LIBRARY

## Generated/Configured headers:
configure_headers(configured_headers
    FILES
        include/${PROJECT_NAMESPACE}/${PROJECT_FEATURE_NAME}/version.hpp.in
)

## Headers:
set(headers
    include/arba/appt/application/application.hpp
    include/arba/appt/application/basic_application.hpp
    include/arba/appt/application/decorator/multi_task.hpp
    include/arba/appt/application/decorator/multi_user.hpp
    include/arba/appt/application/decorator/toolkit.hpp
    include/arba/appt/application/execution_status.hpp
    include/arba/appt/application/module/basic_module.hpp
    include/arba/appt/application/module/concepts/concrete_derived_basic_module.hpp
    include/arba/appt/application/module/decorator/loop.hpp
    include/arba/appt/application/module/decorator/multi_user.hpp
    include/arba/appt/application/module/module.hpp
    include/arba/appt/application/module/module_base.hpp
    include/arba/appt/user/user.hpp
    include/arba/appt/user/user_hash.hpp
    include/arba/appt/user/user_manager.hpp
    include/arba/appt/user/user_set.hpp
    include/arba/appt/util/delta_time.hpp
    include/arba/appt/util/integer_id_factory.hpp
    include/arba/appt/util/logging/log_critical_message.hpp
)

if(${ARBA_APPT_USE_SPDLOG})
    list(APPEND headers
        include/arba/appt/application/decorator/logging.hpp
        include/arba/appt/application/module/decorator/logging.hpp
        include/arba/appt/util/logging/logger_make_helper.hpp
        include/arba/appt/util/logging/logging_macro.hpp
    )
endif()

## Sources:
set(sources
    src/arba/appt/basic_application.cpp
    src/arba/appt/log_critical_message.cpp
    src/arba/appt/module_base.cpp
    src/arba/appt/user.cpp
)

if(${ARBA_APPT_USE_SPDLOG})
    list(APPEND sources
        src/arba/appt/logger_make_helper.cpp
        src/arba/appt/application/decorator/logging.cpp
        src/arba/appt/application/module/decorator/logging.cpp
    )
endif()

## Add C++ library:
cxx_standard_option(${PROJECT_UPPER_VAR_NAME}_CXX_STANDARD MIN 20 MAX 26)

shared_or_static_option(${PROJECT_UPPER_VAR_NAME}_LIBRARY_TYPE "SHARED")
if("${${PROJECT_UPPER_VAR_NAME}_LIBRARY_TYPE}" STREQUAL "SHARED")
  set(LIBRARY_TYPE_POSTFIX "")
  set(PROJECT_TARGET_NAME "${PROJECT_NAME}")
else()
  set(LIBRARY_TYPE_POSTFIX "-static")
  set(PROJECT_TARGET_NAME "${PROJECT_NAME}${LIBRARY_TYPE_POSTFIX}")
endif()

add_cpp_library(${PROJECT_TARGET_NAME} ${${PROJECT_UPPER_VAR_NAME}_LIBRARY_TYPE}
  HEADERS ${headers} ${configured_headers}
  SOURCES ${sources}
  CXX_STANDARD ${${PROJECT_UPPER_VAR_NAME}_CXX_STANDARD}
  DEFAULT_WARNING_OPTIONS
  DEFAULT_ERROR_OPTIONS
)
add_library("${PROJECT_NAMESPACE}::${PROJECT_FEATURE_NAME}${LIBRARY_TYPE_POSTFIX}" ALIAS ${PROJECT_TARGET_NAME})

## Link C++ targets:
find_package(arba-core 0.30.0 REQUIRED CONFIG)
find_package(arba-stdx 0.3.0 REQUIRED CONFIG)
find_package(arba-rsce 0.5.0 REQUIRED CONFIG)
find_package(arba-evnt 0.7.0 REQUIRED CONFIG)
target_link_libraries(${PROJECT_TARGET_NAME}
    PUBLIC
        arba::core
        arba::stdx
        arba::rsce
        arba::evnt
)

if(${ARBA_APPT_USE_SPDLOG})
    find_package(spdlog 1.8.0 REQUIRED CONFIG)
    target_link_libraries(${PROJECT_TARGET_NAME}
        PUBLIC
            spdlog::spdlog
    )
endif()

## Add tests:
add_test_subdirectory_if_build(test)

## Add examples:
add_example_subdirectory_if_build(example)

# C++ INSTALL

## Install C++ library:
install_cpp_libraries(TARGETS ${PROJECT_TARGET_NAME} EXPORT ${PROJECT_NAME}-targets)

## Install project package
install_library_package(${PROJECT_NAME} INPUT_PACKAGE_CONFIG_FILE cmake/config/package-config.cmake.in)
install_uninstall_script(${PROJECT_NAME})
