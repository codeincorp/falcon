###################################################################################################
# Copyright (C) 2021-present Codein Inc.
#
# This program is free software: you can redistribute it and/or modify it under the terms of
# BSD-3-Clause License which can be found at the root directory of this repository.
###################################################################################################

cmake_minimum_required(VERSION 3.0.0)

set(THIS csv_query_test)

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/googletest)

ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

include_directories(../src)

set(TEST_SOURCES
    csv_file_scanner_test.cpp
    any_converter_test.cpp
    any_visitor_test.cpp
    projector_test.cpp
    expression_test.cpp
    conversion_test.cpp
    filter_test.cpp
    metadata_test.cpp
    hash_aggregator_test.cpp
    sequencer_test.cpp
    limiter_test.cpp
    util.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/metadata.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/data.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/metadata1.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/data1.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/metadata2.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/data2.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/empty_fields_data.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/empty_fields_metadata.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/empty_file.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/empty_file.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/invalid_data.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/invalid_metadata.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/data_basic_test.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/metadata_basic_test.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/projector.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/projector.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields1.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields1.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields2.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields2.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields3.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/different_fields3.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/fileScanner_filter_test.csv ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/fileScanner_filter_test.txt ${CMAKE_CURRENT_BINARY_DIR})


add_executable(${THIS} ${TEST_SOURCES})
add_dependencies(${THIS} googletest)
target_link_libraries(${THIS} PUBLIC
    gtest
    gtest_main
    pthread
    csvqry
)

add_test(NAME ${THIS}
    COMMAND ${THIS}
)
