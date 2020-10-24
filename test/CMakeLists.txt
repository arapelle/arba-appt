add_cpp_library_tests(SHARED ${PROJECT_NAME}
                      STATIC ${PROJECT_NAME}-static
                      SOURCES
                          program_args_tests.cpp
                          application_tests.cpp
                          multi_task_application_tests.cpp
                          multi_user_application_tests.cpp
                          multi_task_multi_user_application_tests.cpp
                          integer_id_factory_tests.cpp
                          user_manager_tests.cpp
                          user_hash_tests.cpp
                          user_set_tests.cpp
                          loop_module_tests.cpp
                          logging_tests.cpp
                          event_forwarding_tests.cpp
                      )
