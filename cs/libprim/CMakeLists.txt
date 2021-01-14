add_definitions( -DLIBPRIM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings )
endif()

file(GLOB prim_src "*.h" "*.cpp")
add_library(prim STATIC ${prim_src})
