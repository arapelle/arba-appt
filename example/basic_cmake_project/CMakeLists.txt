cmake_minimum_required(VERSION 3.18)

project(basic_cmake_project)

add_executable(${PROJECT_NAME} main.cpp)
# Find package appt:
find_package(appt 0.10 CONFIG REQUIRED)
# Use appt release shared target:
target_link_libraries(${PROJECT_NAME} PRIVATE arba::appt)
# Use appt release static target:
#target_link_libraries(${PROJECT_NAME} PRIVATE arba::appt-static)
