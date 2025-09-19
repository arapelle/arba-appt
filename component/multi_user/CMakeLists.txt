
# C++ LIBRARY
set_project_name(NAME "multi_user")
project(${PROJECT_NAME})

## Headers:
set(headers
    include/arba/appt/multi_user/application/module/decorator/multi_user.hpp
    include/arba/appt/multi_user/application/decorator/multi_user.hpp
    include/arba/appt/multi_user/user/user_hash.hpp
    include/arba/appt/multi_user/user/user.hpp
    include/arba/appt/multi_user/user/user_manager.hpp
    include/arba/appt/multi_user/user/user_set.hpp
)

## Sources:
set(sources
    src/arba/appt/multi_user/user/user.cpp
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
