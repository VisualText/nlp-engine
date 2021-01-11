add_definitions( -DLIBKBM_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

file(GLOB kbm_src "*.h" "*.cpp")
add_library(kbm STATIC ${kbm_src})
