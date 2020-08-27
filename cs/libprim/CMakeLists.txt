include_directories(../../include/Api)
include_directories(../include)

add_definitions( -DLIBPRIM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

add_library(libprim STATIC
    dir.cpp
    io.cpp
    libprim.cpp
    list.cpp
    prim.cpp
    sm.cpp
    str.cpp
    dyn.cpp
    u_out.cpp
)
