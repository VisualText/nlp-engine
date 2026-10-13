add_definitions( -DWORDS_EXPORTS )
if(WIN32)
  add_definitions( -DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS )
else()
  add_definitions( -DLINUX -Wno-write-strings)
endif()

# The word list itself is data/words/en-words.txt, read on first use (#697);
# this library only holds the loader and the arrays it fills.
add_library(words STATIC
    wordarrs.cpp
)
