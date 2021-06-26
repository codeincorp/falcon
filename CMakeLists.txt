###################################################################################################
# Copyright (C) 2021-present Codein Inc.
#
# This program is free software: you can redistribute it and/or modify it under the terms of
# BSD-3-Clause License which can be found at the root directory of this repository.
###################################################################################################

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
    src/expression.h
    src/filter.h
    src/hash_aggregator.h
    src/sequencer.h
    src/limiter.h
)

set(SOURCES
    src/csv_file_scanner.cpp
    src/to_any_converter.cpp
    src/any_visitor.cpp
    src/projector.cpp
    src/expression.cpp
    src/filter.cpp
    src/hash_aggregator.cpp
    src/sequencer.cpp
    src/limiter.cpp
)

add_library(csvqry STATIC ${SOURCES} ${HEADERS})

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
