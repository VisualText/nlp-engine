#include_directories(/usr/local/libiodbc-3.52.12/include)

set(VERSION ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/version.o)

add_definitions( -DNLP_EXPORTS )
if(WIN32)
  execute_process(COMMAND cmd /c ${CMAKE_SOURCE_DIR}/nlp/makeversion.bat > ${CMAKE_SOURCE_DIR}/nlp/version.cpp)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings -Wno-deprecated)
endif()

add_definitions( -DNLP_EXPORTS )

file(GLOB nlp_src "*.h" "*.cpp")

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(WIN32)
  add_executable(nlp ${nlp_src})
  target_link_libraries(nlp PUBLIC prim kbm consh words lite)
  #target_link_libraries(nlp PUBLIC prim kbm consh words lite odbc)
  add_custom_command(TARGET nlp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/nlp.exe ${CMAKE_SOURCE_DIR}
  )
else()
  add_executable(nlp.exe ${nlp_src})
  target_link_libraries(nlp.exe PUBLIC prim kbm consh words lite ${VERSION})
  #target_link_libraries(nlp.exe PUBLIC prim kbm consh words lite odbc)
  execute_process(COMMAND touch ${VERSION})
  add_custom_command(TARGET nlp.exe
    PRE_LINK COMMAND bash ${CMAKE_SOURCE_DIR}/nlp/makeversion.sh -o ${VERSION}
  )
  add_custom_command(TARGET nlp.exe
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/nlp.exe ${CMAKE_SOURCE_DIR}
  )
endif()
