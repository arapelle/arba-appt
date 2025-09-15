cmake_minimum_required(VERSION 3.15)

project(PackageTest CXX)

set(package_name "arba-appt")

find_package(${package_name} CONFIG REQUIRED
    COMPONENTS
        base
    OPTIONAL_COMPONENTS
        multi_user
        spdlogging
)

set(component_name "base")
set(executable_name "test_package-${component_name}")
add_executable(${executable_name} ${executable_name}.cpp)
target_link_libraries(${executable_name}
    PRIVATE
        ${package_name}::${package_name}
)

set(component_name "multi_user")
if(TARGET "${package_name}::${component_name}")
    set(executable_name "test_package-${component_name}")
    add_executable(${executable_name} ${executable_name}.cpp)
    target_link_libraries(${executable_name}
        PRIVATE
            ${package_name}::${package_name}
    )
endif()

set(component_name "spdlogging")
if(TARGET "${package_name}::${component_name}")
    set(executable_name "test_package-${component_name}")
    add_executable(${executable_name} ${executable_name}.cpp)
    target_link_libraries(${executable_name}
        PRIVATE
            ${package_name}::${package_name}
    )
endif()
