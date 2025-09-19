
# C++ LIBRARY
add_library(${PACKAGE_NAME} INTERFACE)
target_link_libraries(${PACKAGE_NAME} INTERFACE base)
foreach(component ${PACKAGE_COMPONENTS})
    if(TARGET ${component})
        target_link_libraries(${PACKAGE_NAME} INTERFACE ${component})
    endif()
endforeach()

# Install component targets:
install_cpp_targets(TARGETS ${PACKAGE_NAME} EXPORT ${PACKAGE_NAME}-targets NAMESPACE "${PACKAGE_NAME}::")

## Add examples:
if(${${PACKAGE_UPPER_VAR_NAME}_BUILD_EXAMPLES})
    add_subdirectory(example)
endif()
