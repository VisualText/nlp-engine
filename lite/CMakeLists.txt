#include_directories(/usr/local/libiodbc-3.52.12/include)

add_definitions( -DLITE_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings -Wno-deprecated )
endif()

file(GLOB lite_src "*.h" "*.cpp")
add_library(lite STATIC ${lite_src})
target_link_libraries(lite PUBLIC 
  prim
  kbm
  consh
  words
  ${ICU_LIBRARIES})
