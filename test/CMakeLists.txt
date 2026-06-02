if(WIN32)
    add_definitions(-DMSVC_VERSION=${MSVC_VERSION} -D_CRT_SECURE_NO_WARNINGS)
else()
    add_definitions(-DLINUX -Wno-write-strings -Wno-deprecated)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT WIN32)
    find_library(DL_LIBRARY dl)
endif()

if(WIN32)
    add_executable(test-nlp main.cpp)
    target_link_libraries(test-nlp prim kbm consh words lite)
    # NLP-ENGINE-522: use a generator expression for the source path so
    # this works under both single-config (Makefiles / Ninja, where
    # CMAKE_BUILD_TYPE is set at configure time) AND multi-config
    # generators (Visual Studio, where CMAKE_BUILD_TYPE is empty and the
    # config is chosen at build time). The original
    #   ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/test-nlp.exe
    # expanded to `bin//test-nlp.exe` under VS and the copy failed,
    # taking the whole build with it — which is what blocked
    # py-package-nlpengine from building locally on Windows.
    add_custom_command(TARGET test-nlp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:test-nlp>
            ${CMAKE_SOURCE_DIR}/bin/test-nlp.exe)
else()
    add_executable(test-nlp.exe main.cpp)
    target_link_libraries(test-nlp.exe prim kbm consh words lite ${ICU_LIBRARIES} ${DL_LIBRARY})
    # NLP-ENGINE-522: the non-Windows branch was copying a file onto
    # itself (`bin/test-nlp.exe -> bin/test-nlp.exe`). Use the actual
    # target output path so this self-copy noise goes away too.
    add_custom_command(TARGET test-nlp.exe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:test-nlp.exe>
            ${CMAKE_SOURCE_DIR}/bin/test-nlp.exe)
endif()
