include(cmtk/CppLibraryProject)

# C++ LIBRARY
project("${PACKAGE_NAME}-ut")

## Headers:
set(headers
    include/arba-appt-ut/modules/counting_module.hpp
    include/arba-appt-ut/modules/bad_crtp_module.hpp
    include/arba-appt-ut/util/stream_capture.hpp
    include/arba-appt-ut/util/text_file_content.hpp
)

## Add C++ library:
add_cpp_library(${PROJECT_NAME} HEADER_ONLY
    HEADERS ${headers}
    CXX_STANDARD ${${PACKAGE_UPPER_VAR_NAME}_CXX_STANDARD}
    DEFAULT_WARNING_OPTIONS
    DEFAULT_ERROR_OPTIONS
)
