include(cmtk/CppLibraryProject)

# C++ LIBRARY
project("${PACKAGE_NAME}-${PROJECT_NAME}-ut")

## Headers:
set(headers
    include/arba-appt-standard-ut/modules/counting_module.hpp
    include/arba-appt-standard-ut/modules/bad_crtp_module.hpp
)

## Add C++ library:
add_cpp_library(${PROJECT_NAME} HEADER_ONLY
    HEADERS ${headers}
    CXX_STANDARD ${${PACKAGE_UPPER_VAR_NAME}_CXX_STANDARD}
    DEFAULT_WARNING_OPTIONS
    DEFAULT_ERROR_OPTIONS
)

target_link_libraries(${PROJECT_NAME} INTERFACE ${PACKAGE_NAME}-base-ut)
