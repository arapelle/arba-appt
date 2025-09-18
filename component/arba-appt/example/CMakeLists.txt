include(cmtk/CppLibraryExamples)

if(TARGET base AND TARGET standard AND TARGET multi_user AND TARGET spdlogging)
    add_cpp_library_basic_examples(${PACKAGE_NAME}
        SOURCES
            multi_task_multi_user_logging_application_example.cpp
    )
endif()
