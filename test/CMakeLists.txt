find_or_fetch_GTest()

add_cpp_library_basic_tests(GTest::gtest_main
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    SOURCES
        project_version_tests.cpp
        application_tests.cpp
        event_forwarding_tests.cpp
        exception_handling_tests.cpp
        execution_status_tests.cpp
        format_time_point_tests.cpp
        integer_id_factory_tests.cpp
        logging_tests.cpp
        loop_module_tests.cpp
        multi_task_application_sm_tests.cpp
        multi_task_application_mm_tests.cpp
        multi_task_multi_user_application_tests.cpp
        multi_user_application_tests.cpp
        program_args_tests.cpp
        user_hash_tests.cpp
        user_manager_tests.cpp
        user_set_tests.cpp
)
