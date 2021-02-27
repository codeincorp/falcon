cmake_minimum_required(VERSION 3.0.0)

set(THIS csv_scanner_test)

include_directories(../src)

add_executable(${THIS} CsvFileScannerTest.cpp)
target_link_libraries(${THIS} PUBLIC
    gtest
    gtest_main
    pthread
    csvqry
)

add_test(NAME ${THIS}
    COMMAND ${THIS}
)
