configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)


set(RESOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/res/)

include_directories(${HEADER_DIRECTORY} 
                    ${RESOURCE_DIRECTORY})


set(SOURCES 
  main.cpp 
  flagResolverTests.cpp
  socketWrapperTests.cpp
  sourceTest.cpp
  lexicalAnalyzerTest.cpp
  ${SOURCE_DIRECTORY}/program.cpp
  ${SOURCE_DIRECTORY}/source.cpp
  ${SOURCE_DIRECTORY}/${HELPERS_DIRECTORY}/sourceFactory.cpp
  ${SOURCE_DIRECTORY}/${HELPERS_DIRECTORY}/position.cpp
  ${LEXICAL_ANALYZER_DIRECTORY}lexicalAnalyzer.cpp
)

add_executable(tests ${SOURCES})
target_link_libraries(tests gtest gtest_main)