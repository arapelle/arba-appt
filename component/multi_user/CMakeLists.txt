
# C++ LIBRARY
set_project_name(NAME "multi_user")
project(${PROJECT_NAME})

## Headers:
set(headers
    include/arba/appt/application/module/decorator/multi_user/multi_user.hpp
    include/arba/appt/application/decorator/multi_user/multi_user.hpp
    include/arba/appt/user/user_hash.hpp
    include/arba/appt/user/user.hpp
    include/arba/appt/user/user_manager.hpp
    include/arba/appt/user/user_set.hpp
)

## Sources:
set(sources
    src/arba/appt/user/user.cpp
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
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_TESTS})
    add_test_subdirectory_if_built(test OPTION_DEFAULT ON)
endif()

## Add examples:
if(${BUILD_${PACKAGE_UPPER_VAR_NAME}_EXAMPLES})
    add_example_subdirectory_if_built(example OPTION_DEFAULT ON)
endif()
