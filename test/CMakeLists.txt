if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings -Wno-deprecated)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(WIN32)
  add_executable(test-nlp main.cpp)
  target_link_libraries(test-nlp prim kbm consh words lite)
  add_custom_command(TARGET test-nlp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/test-nlp.exe ${CMAKE_SOURCE_DIR})
else()
  add_executable(test-nlp.exe main.cpp)
  target_link_libraries(test-nlp.exe prim kbm consh words lite)
  add_custom_command(TARGET test-nlp.exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/test-nlp.exe ${CMAKE_SOURCE_DIR})
endif()
