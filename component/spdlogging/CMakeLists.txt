
# C++ LIBRARY
set_project_name(NAME "spdlogging")
project(${PROJECT_NAME})

## Headers:
set(headers
    include/arba/appt/application/module/decorator/spdlogging/spdlogging.hpp
    include/arba/appt/application/decorator/spdlogging/spdlogging.hpp
    include/arba/appt/util/spdlogging/spdlog_helper.hpp
    include/arba/appt/util/spdlogging/logging_macro.hpp
)

## Sources:
set(sources
    src/arba/appt/application/module/decorator/spdlogging/spdlogging.cpp
    src/arba/appt/application/decorator/spdlogging/spdlogging.cpp
    src/arba/appt/util/spdlogging/spdlog_helper.cpp
)

## Add C++ library:
add_cpp_library(${PROJECT_NAME} ${${PACKAGE_UPPER_VAR_NAME}_LIBRARY_TYPE}
    HEADERS ${headers}
    SOURCES ${sources}
    CXX_STANDARD ${${PACKAGE_UPPER_VAR_NAME}_CXX_STANDARD}
    DEFAULT_WARNING_OPTIONS
    DEFAULT_ERROR_OPTIONS
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        base
        spdlog::spdlog
)

## Install component targets:
install_cpp_targets(TARGETS ${PROJECT_NAME} EXPORT ${PACKAGE_NAME}-targets NAMESPACE "${PACKAGE_NAME}::")

## Add tests:
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_TESTS})
    add_test_subdirectory_if_built(test OPTION_DEFAULT ON)
endif()

## Add examples:
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_EXAMPLES})
    add_example_subdirectory_if_built(example OPTION_DEFAULT ON)
endif()
