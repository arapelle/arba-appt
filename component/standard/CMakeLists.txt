
# C++ LIBRARY
set_project_name(NAME "standard")
project(${PROJECT_NAME})

## Generated/Configured headers:
configure_headers(configured_headers
    FILES
        include/${PACKAGE_CONTEXT_NAME}/${PACKAGE_SUBJECT_NAME}/version.hpp.in
)

## Headers:
set(headers
    include/arba/appt/standard/application/application.hpp
    include/arba/appt/standard/application/module/module.hpp
    include/arba/appt/standard/application/decorator/toolkit.hpp
)

## Add C++ library:
add_cpp_library(${PROJECT_NAME} HEADER_ONLY
    HEADERS ${headers}
    CXX_STANDARD ${${PACKAGE_UPPER_VAR_NAME}_CXX_STANDARD}
    DEFAULT_WARNING_OPTIONS
    DEFAULT_ERROR_OPTIONS
)
target_link_libraries(${PROJECT_NAME}
    INTERFACE
        base
        arba::rsce
        arba::evnt
)

# Install component targets:
install_cpp_targets(TARGETS ${PROJECT_NAME} EXPORT ${PACKAGE_NAME}-targets NAMESPACE "${PACKAGE_NAME}::")

## Add tests:
if(${${PACKAGE_UPPER_VAR_NAME}_BUILD_TESTS})
    add_test_subdirectory_if_built(test OPTION_DEFAULT ON)
endif()

## Add examples:
if(${${PACKAGE_UPPER_VAR_NAME}_BUILD_EXAMPLES})
    add_example_subdirectory_if_built(example OPTION_DEFAULT ON)
endif()
