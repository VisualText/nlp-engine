add_definitions( -DLIBCONSH_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings )
endif()

file(GLOB consh_src "*.cpp")
add_library(consh STATIC ${consh_src})

target_link_libraries(consh PUBLIC prim kbm)
