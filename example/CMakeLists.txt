include(cmtk/CppLibraryExamples)

add_cpp_library_basic_examples(${PROJECT_TARGET_NAME}
    SOURCES
        program_args_example.cpp
        logging_application_example.cpp
        multi_task_application_example.cpp
        multi_task_multi_user_application_example.cpp
        multi_task_multi_user_logging_application_example.cpp
)
