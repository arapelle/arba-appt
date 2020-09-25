add_cpp_library_tests(SHARED ${PROJECT_NAME}
                      STATIC ${PROJECT_NAME}-static
                      SOURCES
                          program_args_tests.cpp
                          application_tests.cpp
                      )
