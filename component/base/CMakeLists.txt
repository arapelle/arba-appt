
# C++ LIBRARY
set_project_name(NAME "base")
project(${PROJECT_NAME})

## Generated/Configured headers:
configure_headers(configured_headers
    FILES
        include/${PACKAGE_CONTEXT_NAME}/${PACKAGE_SUBJECT_NAME}/version.hpp.in
)

## Headers:
set(headers
    include/arba/appt/application/application.hpp
    include/arba/appt/application/module/module_base.hpp
    include/arba/appt/application/module/basic_module.hpp
    include/arba/appt/application/module/decorator/loop.hpp
    include/arba/appt/application/module/module.hpp
    include/arba/appt/application/module/concepts/concrete_derived_basic_module.hpp
    include/arba/appt/application/decorator/multi_task.hpp
    include/arba/appt/application/decorator/toolkit.hpp
    include/arba/appt/application/execution_status.hpp
    include/arba/appt/application/basic_application.hpp
    include/arba/appt/util/delta_time.hpp
    include/arba/appt/util/log_critical_message.hpp
    include/arba/appt/util/integer_id_factory.hpp
)

## Sources:
set(sources
    src/arba/appt/application/module/module_base.cpp
    src/arba/appt/application/basic_application.cpp
    src/arba/appt/util/log_critical_message.cpp
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
        arba::core
        arba::stdx
        arba::rsce
        arba::evnt
)

# Install component targets:
install_cpp_targets(TARGETS ${PROJECT_NAME} EXPORT ${PACKAGE_NAME}-targets NAMESPACE "${PACKAGE_NAME}::")

## Add tests:
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_TESTS})
    add_test_subdirectory_if_built(test)
endif()

## Add examples:
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_EXAMPLES})
    add_example_subdirectory_if_built(example)
endif()
