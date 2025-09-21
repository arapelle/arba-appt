include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_NAME} GTest::gtest_main
    SOURCES
        logging_tests.cpp
        exception_handling_with_spdlog_tests.cpp
    DEPENDENCIES
        ${PACKAGE_NAME}-base-ut
)
