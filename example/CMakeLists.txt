add_cpp_library_examples(
    SHARED ${PROJECT_NAME}
    STATIC ${PROJECT_NAME}-static
    SOURCES
        program_args_example.cpp
        logging_application_example.cpp
        multi_task_application_example.cpp
        multi_task_multi_user_application_example.cpp
        multi_task_multi_user_logging_application_example.cpp
)
