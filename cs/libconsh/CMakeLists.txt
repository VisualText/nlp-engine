include_directories(../../include/Api)
include_directories(../include)

add_definitions( -DLIBCONSH_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

add_library(libconsh STATIC
    Aconsh.cpp
    arg.cpp
    bind.cpp
    cc_code.cpp
    cc_gen.cpp
    cg.cpp
    cg_global.cpp
    cmd.cpp
    con_gen.cpp
    con_ini.cpp
    consh_gen.cpp
    consh_kb.cpp
    dyn.cpp
    gen.cpp
    help.cpp
    ind.cpp
    io.cpp
    libconsh.cpp
    ptr_gen.cpp
    ptr_ini.cpp
    st_gen.cpp
    st_ini.cpp
    sym_gen.cpp
    sym_ini.cpp
    u_out.cpp
    ui.cpp
)
