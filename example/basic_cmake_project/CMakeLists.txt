cmake_minimum_required(VERSION 3.26)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME}-static main.cpp)
# Find package arba-appt:
find_package(arba-appt 0.12.0 CONFIG REQUIRED)
# Use arba-appt release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE arba-appt)
# Use arba-appt release static target:
target_link_libraries(${PROJECT_NAME}-static PRIVATE arba-appt-static)
