cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_definitions(-DANTLR4CPP_STATIC)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE /usr/local/lib/antlr-4.9.2-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(SampleGrammarLexer TLexer.g4 LEXER
             PACKAGE antlrcpptest)
antlr_target(SampleGrammarParser TParser.g4 PARSER
             PACKAGE antlrcpptest
             DEPENDS_ANTLR SampleGrammarLexer
             COMPILE_FLAGS -lib ${ANTLR_SampleGrammarLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_SampleGrammarLexer_OUTPUT_DIR})
include_directories(${ANTLR_SampleGrammarParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_executable(demo antlr4demo.cpp
               ${ANTLR_SampleGrammarLexer_CXX_OUTPUTS}
               ${ANTLR_SampleGrammarParser_CXX_OUTPUTS})
target_link_libraries(demo antlr4_static)

set(HEADERS
    metadata.h
    iterator.h
    csv_file_scanner.h
    to_any_converter.h
    any_visitor.h
    projector.h
    expression.h
    filter.h
    hash_aggregator.h
    sequencer.h
    limiter.h
)

set(SOURCES
    csv_file_scanner.cpp
    to_any_converter.cpp
    any_visitor.cpp
    projector.cpp
    expression.cpp
    filter.cpp
    hash_aggregator.cpp
    sequencer.cpp
    limiter.cpp
)

add_library(csvqry STATIC ${SOURCES} ${HEADERS})
