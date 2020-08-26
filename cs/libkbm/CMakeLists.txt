include_directories(../../include/Api)
include_directories(../include)

add_definitions( -DLIBKBM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

add_library(libkbm STATIC
    con_.cpp
    Akbm.cpp
    st.cpp
    attr.cpp
    dict.cpp
    find.cpp
    kbm_alloc.cpp
    kbm.cpp
    kbm_kb.cpp
    libkbm.cpp
    pret.cpp
    ptr.cpp
    sym.cpp
)
