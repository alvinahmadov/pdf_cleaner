cmake_minimum_required(VERSION 3.15)
project(pdf_sanitizer)

set(CMAKE_CXX_STANDARD 17)

include(FindPkgConfig)
pkg_check_modules(PODOFO libpodofo)

find_package(Boost 1.65 COMPONENTS filesystem system program_options)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Wattributes -Wextra -O3 -ffp-contract=off -march=native -pthread")

set(PODOFO_INCLUDE_DIRS ${PODOFO_INCLUDE_DIRS})
set(PODOFO_LIBRARIES ${PODOFO_LIBRARIES})

set(PDFCLEANER_LIB ${CMAKE_PROJECT_NAME})
set(PROJECT_INCLUDE_DIRS src ${PODOFO_INCLUDE_DIRS})
set(PROJECT_LIBRARIES ${PODOFO_LIBRARIES}
        Boost::filesystem
        Boost::system
        Boost::program_options)

add_subdirectory(src)

add_executable(pdfsanitizer src/main.cpp)
target_include_directories(pdfsanitizer PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries(pdfsanitizer PUBLIC ${PDFCLEANER_LIB})
