add_definitions( -DLIBPRIM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings )
endif()

file(GLOB stream_src "u_convert.*")
add_library(stream STATIC ${stream_src})
