cmake_minimum_required(VERSION 3.0.0)
project(nlp-engine VERSION 0.1.0)

include(CTest)
enable_testing()

set(NLP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cs)
add_subdirectory(src)
add_subdirectory(lite)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
