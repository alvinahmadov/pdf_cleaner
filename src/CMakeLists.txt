file(GLOB PDFCLEANER_SOURCES pdf/*.cpp pdf/**/*.cpp pdf/*.hpp pdf/**/*.hpp)

add_library(${PDFCLEANER_LIB} STATIC ${PDFCLEANER_SOURCES})
target_include_directories(${PDFCLEANER_LIB} PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${PDFCLEANER_LIB} PUBLIC ${PROJECT_LIBRARIES})