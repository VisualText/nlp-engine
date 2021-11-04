add_definitions( -DLIBSTREAM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings )
endif()

file(GLOB libstream_src "*.h" "*.cpp")
add_library(stream STATIC ${libstream_src})
