cmake_minimum_required(VERSION 3.0.0)
project(csv_query VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(HEADERS
    src/metadata.h
    src/iterator.h
    src/csv_file_scanner.h
    src/to_any_converter.h
    src/any_visitor.h
    src/projector.h
)

set(SOURCES
    src/csv_file_scanner.cpp
    src/to_any_converter.cpp
    src/any_visitor.cpp
    src/projector.cpp
)

add_executable(csv_query src/csv_query.cpp ${SOURCES} ${HEADERS})

add_library(csvqry STATIC ${SOURCES} ${HEADERS})

# add_subdirectory(googletest)
add_subdirectory(test)

add_executable(opt_n_any src/opt_n_any.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
