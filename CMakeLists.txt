cmake_minimum_required(VERSION 3.15)
project(pdf_cleaner)

set(CMAKE_CXX_STANDARD 17)

include(FindPkgConfig)
pkg_check_modules(PODOFO podofo)

find_package(Boost 1.65 COMPONENTS filesystem system program_options)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -O3 -ffp-contract=off -march=native")

set(PDFCLEANER_LIB ${CMAKE_PROJECT_NAME})
set(PROJECT_INCLUDE_DIRS src ${PODOFO_INCLUDE_DIRS})
set(PROJECT_LIBRARIES ${PODOFO_LIBRARIES}
    Boost::filesystem
    Boost::system
    Boost::program_options)

add_subdirectory(src)
add_executable(pdfuriremove src/main.cpp src/Defines.hpp)
target_include_directories(pdfuriremove PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries(pdfuriremove PUBLIC ${PDFCLEANER_LIB})