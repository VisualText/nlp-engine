#include_directories(${NLP_SRC_DIR}/include/Api/lite)
#include_directories(/usr/local/libiodbc-3.52.12/include)

add_definitions( -DLIBNLPLIB_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX )
endif()

file(GLOB test_src "*.h" "*.cpp")

add_library(nlplib STATIC ${test_src})

target_link_libraries(nlplib prim kbm consh words lite odbc)
