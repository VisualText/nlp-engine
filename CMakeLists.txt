cmake_minimum_required(VERSION 3.0.0)
project(nlp VERSION 0.1.0)

include(CTest)
enable_testing()

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

set(CMAKE_GENERATOR_PLATFORM Win32)

if(MSVC)
    add_compile_options(/w44244 /w44311 /w44302 /w44267 /w44312)
    add_definitions(-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)
    add_definitions(-DNONSTD_)
    add_definitions(-D_CONSOLE)
    add_definitions(-DWIN32)
    add_definitions(-DNDEBUG)
    add_definitions(-D_WINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t")
    find_program(CMAKE_MAKE_PROGRAM
        NAMES mingw32-make
              mingw32-make.exe
        DOC "Find a suitable make program for building under Windows/MinGW"
        HINTS c:/MinGW-32/bin )
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
include_directories(icu/include)
include_directories(icu/source/include)
include_directories(icu/source/common/unicode)

add_subdirectory(icu/source)
add_subdirectory(cs)
add_subdirectory(src)
add_subdirectory(lite)
add_subdirectory(nlp)
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
