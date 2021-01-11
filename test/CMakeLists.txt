#include_directories(/usr/local/libiodbc-3.52.12/include)

if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings -Wno-deprecated)
endif()

add_executable(test-nlp main.cpp)
target_link_libraries(test-nlp prim kbm consh words lite)
#target_link_libraries(test-nlp PUBLIC prim kbm consh words lite odbc)
