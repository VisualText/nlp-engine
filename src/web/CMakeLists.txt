add_definitions( -DWEB_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS -D_AFXDLL )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

add_library(web STATIC
    StdAfx.cpp
    tear.cpp
    url.cpp
    urls.cpp
    util.cpp
    web.cpp
)
