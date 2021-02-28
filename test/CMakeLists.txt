cmake_minimum_required(VERSION 3.0.0)

set(THIS csv_query_test)

include_directories(../src)

set(TEST_SOURCES
    CsvFileScannerTest.cpp
    any_converter_test.cpp
    any_visitor_test.cpp
)

add_executable(${THIS} ${TEST_SOURCES})
target_link_libraries(${THIS} PUBLIC
    gtest
    gtest_main
    pthread
    csvqry
)

add_test(NAME ${THIS}
    COMMAND ${THIS}
)
