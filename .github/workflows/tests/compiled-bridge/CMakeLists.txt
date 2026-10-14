# Builds this fixture as a compiled analyzer (analyzer.dll) against the
# packaged compile libraries, as the VS Code extension and the compile service
# do (their generated CMakeLists.txt is the model). Used by the Windows CI step
# "Compiled analyzer calls interpreter-only builtins".
#
#   nlp -COMPILEANA -ANA <this folder> ...     (writes run/*.cpp)
#   cmake -S <this folder> -B <this folder>/build -A x64 -DENGINE_LIBS=<compile-libs>
#   cmake --build <this folder>/build --config Release
cmake_minimum_required(VERSION 3.16)
project(nlp_generated_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ENGINE_LIBS)
    message(FATAL_ERROR "Pass -DENGINE_LIBS=<unpacked nlpengine-compile-libs folder>.")
endif()

set(ANALYZER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ANALYZER_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ANALYZER_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ANALYZER_DIR}")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${ANALYZER_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${ANALYZER_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${ANALYZER_DIR}")
endforeach()

file(GLOB GENERATED_CPP "${ANALYZER_DIR}/run/*.cpp" "${ANALYZER_DIR}/kb/*.cpp")
if(NOT GENERATED_CPP)
    message(FATAL_ERROR "No generated C++ in run/ or kb/. Run nlp -COMPILEANA first.")
endif()

add_library(nlp_generated SHARED ${GENERATED_CPP})
set_target_properties(nlp_generated PROPERTIES OUTPUT_NAME "analyzer" PREFIX "")

target_include_directories(nlp_generated PRIVATE
    "${ANALYZER_DIR}"
    "${ANALYZER_DIR}/run"
    "${ANALYZER_DIR}/kb"
    "${ENGINE_LIBS}/include/Api"
    "${ENGINE_LIBS}/include/cs"
    "${ENGINE_LIBS}/include"
)

file(GLOB ENGINE_LIBS_ALL "${ENGINE_LIBS}/lib/*.lib")
target_link_libraries(nlp_generated PRIVATE ${ENGINE_LIBS_ALL})
target_compile_definitions(nlp_generated PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(nlp_generated PRIVATE /wd4005 /FI"StdAfx.h" /MP)
