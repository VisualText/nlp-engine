include_directories(../include/Api/lite)
include_directories(../include/Api)
include_directories(../cs/include)
#include_directories(/usr/local/libiodbc-3.52.12/include)

add_definitions( -DNLP_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings -Wno-deprecated)
endif()

file(GLOB nlp_src "*.h" "*.cpp")
add_executable(nlp.exe ${nlp_src})

target_link_libraries(nlp.exe PUBLIC prim kbm consh words lite)
#target_link_libraries(nlp.exe PUBLIC prim kbm consh words lite odbc)