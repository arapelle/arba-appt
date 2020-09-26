add_cpp_library_tests(SHARED ${PROJECT_NAME}
                      STATIC ${PROJECT_NAME}-static
                      SOURCES
                          program_args_tests.cpp
                          application_tests.cpp
                          multi_task_application_tests.cpp
                          multi_user_application_tests.cpp
                          multi_user_multi_task_application_tests.cpp
                      )
