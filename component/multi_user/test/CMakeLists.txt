include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_NAME} GTest::gtest_main
    SOURCES
        multi_task_multi_user_application_tests.cpp
        multi_user_application_tests.cpp
        user_hash_tests.cpp
        user_manager_tests.cpp
        user_set_tests.cpp
    DEPENDENCIES
        ${PACKAGE_NAME}-ut
)
