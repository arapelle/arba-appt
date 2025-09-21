add_subdirectory(arba-appt-standard-ut)

include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_NAME} GTest::gtest_main
    SOURCES
        application_tests.cpp
        event_forwarding_tests.cpp
        multi_task_application_sm_tests.cpp
        multi_task_application_mm_tests.cpp
    DEPENDENCIES
        ${PACKAGE_NAME}-${PROJECT_NAME}-ut
)
