cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -g1 -lSegFault -Werror -Wpedantic")

project(TKOM VERSION 1.0
        DESCRIPTION "Very nice project"
        LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin/)
set(HEADER_DIRECTORY ${PROJECT_SOURCE_DIR}/include/)
set(SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src/)
set(LEXICAL_ANALYZER_DIRECTORY ${PROJECT_SOURCE_DIR}/src/lexical_analyzer/)
set(HELPERS_DIRECTORY /helpers/)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

include_directories(${HEADER_DIRECTORY})

add_subdirectory(tests)

set(SOURCES 
        ${SOURCE_DIRECTORY}/main.cpp
        ${SOURCE_DIRECTORY}/program.cpp
        ${SOURCE_DIRECTORY}/source.cpp
        ${SOURCE_DIRECTORY}/${HELPERS_DIRECTORY}/sourceFactory.cpp
        ${LEXICAL_ANALYZER_DIRECTORY}lexicalAnalyzer.cpp
        ${SOURCE_DIRECTORY}/${HELPERS_DIRECTORY}/position.cpp
)

add_executable(TKOM ${SOURCES})