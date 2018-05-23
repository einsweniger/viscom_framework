if(MSVC)
    set(BASS_LIB ${CMAKE_SOURCE_DIR}/extern/bass/x64/bass.lib)
    set(BASS_DLL ${CMAKE_SOURCE_DIR}/extern/bass/x64/bass.dll)
    list(APPEND CORE_LIBS ${BASS_LIB})
elseif(UNIX)
    set(BASS_LIB ${CMAKE_SOURCE_DIR}/extern/bass/x64/libbass.so)
    set(BASS_DLL ${CMAKE_SOURCE_DIR}/extern/bass/x64/libbass.so)
    list(APPEND CORE_LIBS ${BASS_LIB})
endif()

list(APPEND CORE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extern/bass/)

#TODO copy bass.dll to build folder?