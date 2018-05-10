if(MSVC)
    list(APPEND CORE_LIBS ${CMAKE_SOURCE_DIR}/extern/bass/x64/bass.lib)
elseif(UNIX)
    list(APPEND CORE_LIBS ${CMAKE_SOURCE_DIR}/extern/bass/x64/libbass.so)
endif()

list(APPEND CORE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extern/bass/)

#TODO copy bass.dll to build folder?