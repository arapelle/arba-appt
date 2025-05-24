include(cmtk/CppLibraryExamples)

add_cpp_library_basic_examples(${PROJECT_TARGET_NAME}
    SOURCES
        multi_task_application_example.cpp
        multi_task_multi_user_application_example.cpp
)

if(${ARBA_APPT_USE_SPDLOG})
    add_cpp_library_basic_examples(${PROJECT_TARGET_NAME}
        SOURCES
            logging_application_example.cpp
            multi_task_multi_user_logging_application_example.cpp
    )
endif()
