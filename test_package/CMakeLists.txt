cmake_minimum_required(VERSION 3.15)

project(PackageTest CXX)

set(test_program "test_package")
add_executable(${test_program}
    test_package.cpp
)

set(package_name "arba-appt")
find_package(${package_name} CONFIG COMPONENTS REQUIRED basic feature) # toto OPTIONAL_COMPONENTS titi)

if(TARGET "${package_name}::${package_name}")
    set(lib_target "${package_name}::${package_name}")
else()
    set(lib_target "${package_name}::${package_name}-static")
endif()

target_link_libraries(${test_program}
    PRIVATE
#        ${lib_target}
        ${package_name}::basic
        ${package_name}::feature
)

target_compile_features(${test_program} PRIVATE cxx_std_20)
