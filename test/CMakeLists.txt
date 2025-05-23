include(cmtk/CppLibraryTests)
include(GoogleTest)

find_package(GTest 1.14 CONFIG REQUIRED)

add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest_main
    SOURCES
        project_version_tests.cpp
        application_tests.cpp
        delta_time_tests.cpp
        event_forwarding_tests.cpp
        exception_handling_tests.cpp
        execution_status_tests.cpp
        integer_id_factory_tests.cpp
        loop_module_tests.cpp
        multi_task_application_sm_tests.cpp
        multi_task_application_mm_tests.cpp
        multi_task_multi_user_application_tests.cpp
        multi_user_application_tests.cpp
        user_hash_tests.cpp
        user_manager_tests.cpp
        user_set_tests.cpp
)

if(${ARBA_APPT_USE_SPDLOG})
    add_cpp_library_basic_tests(${PROJECT_TARGET_NAME} GTest::gtest_main
        SOURCES
            logging_tests.cpp
            exception_handling_with_spdlog_tests.cpp
    )
endif()
