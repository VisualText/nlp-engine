add_definitions( -DLIBPRIM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings )
endif()

file(GLOB prim_src "*.h" "*.cpp")
add_library(prim STATIC ${prim_src})

# dyn.cpp uses dlopen/dlclose on Linux; propagate -ldl to consumers.
if(NOT WIN32)
  target_link_libraries(prim PUBLIC ${CMAKE_DL_LIBS})
endif()
