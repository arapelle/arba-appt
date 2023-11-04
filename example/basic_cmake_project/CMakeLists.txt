cmake_minimum_required(VERSION 3.26)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} basic_cmake_project.cpp)
add_executable(${PROJECT_NAME}-static basic_cmake_project.cpp)
# Find package arba-appt:
find_package(arba-appt 0.14.0 CONFIG REQUIRED)
# Use arba-appt release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE arba-appt)
# Use arba-appt release static target:
target_link_libraries(${PROJECT_NAME}-static PRIVATE arba-appt-static)
