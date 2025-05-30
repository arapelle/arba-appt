
# C++ LIBRARY
set(COMPONENT_NAME "feature")
set(PROJECT_COMPONENT_NAME "${PROJECT_NAME}-feature")
set(PROJECT_TARGET_NAME "${PROJECT_COMPONENT_NAME}${LIBRARY_TYPE_POSTFIX}")

## Headers:
set(headers
    include/arba/appt/appinfo.hpp
)

## Sources:
set(sources
    src/arba/appt/appinfo.cpp
)

## Add C++ library:
add_cpp_library(${PROJECT_TARGET_NAME} ${${PROJECT_UPPER_VAR_NAME}_LIBRARY_TYPE}
  HEADERS ${headers}
  SOURCES ${sources}
  CXX_STANDARD ${${PROJECT_UPPER_VAR_NAME}_CXX_STANDARD}
  DEFAULT_WARNING_OPTIONS
  DEFAULT_ERROR_OPTIONS
)
add_library("${PROJECT_NAMESPACE}::${PROJECT_FEATURE_NAME}-${COMPONENT_NAME}${LIBRARY_TYPE_POSTFIX}" ALIAS ${PROJECT_TARGET_NAME})

install(TARGETS ${PROJECT_TARGET_NAME} EXPORT ${PROJECT_TARGET_NAME}-targets
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
install(EXPORT ${PROJECT_TARGET_NAME}-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
export(EXPORT ${PROJECT_TARGET_NAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_TARGET_NAME}-targets.cmake)


## Link C++ targets:
#find_package(arba-core 0.30.0 REQUIRED CONFIG)
#find_package(arba-stdx 0.3.0 REQUIRED CONFIG)
#find_package(arba-rsce 0.5.0 REQUIRED CONFIG)
#find_package(arba-evnt 0.7.0 REQUIRED CONFIG)
#target_link_libraries(${PROJECT_TARGET_NAME}
#    PUBLIC
#        arba::core
#        arba::stdx
#        arba::rsce
#        arba::evnt
#)

## Add tests:
#add_test_subdirectory_if_build(test)

## Add examples:
#add_example_subdirectory_if_build(example)

# C++ INSTALL

## Install C++ library:
#install_cpp_libraries(TARGETS ${PROJECT_TARGET_NAME} EXPORT ${PROJECT_NAME}-targets)
