cmake_minimum_required(VERSION 3.5.0)
project(nlp VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(ICU REQUIRED COMPONENTS i18n uc data io)
set(CMAKE_GENERATOR_PLATFORM Win32 HINT)

if(MSVC)
    add_compile_options(/w44005 /w44244 /w44311 /w44211 /w44302 /w44267 /w44312 /w45033 /w44624 /w44996 /w44273)
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    add_definitions(-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)
    add_definitions(-DNONSTD_)
    add_definitions(-D_CONSOLE)
    add_definitions(-DWIN32)
    add_definitions(-DNDEBUG)
    add_definitions(-D_WINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /std:c++17")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(NLP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# build shared library (DLL in Windows)
# add_definitions(-DSHARED_LIB)

include_directories(cs/include)
include_directories(include)
include_directories(include/Api)
include_directories(include/Api/lite)
include_directories(${ICU_INCLUDE_DIR})

add_subdirectory(cs)
add_subdirectory(src)
add_subdirectory(lite)
add_subdirectory(nlp)
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
