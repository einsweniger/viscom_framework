# Build-flags.

target_compile_options(glbinding INTERFACE "-Wno-deprecated-declarations")

if(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wno-unused-function -Wno-unused-parameter -Wno-ignored-qualifiers -Wextra -Wpedantic -pg")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      list(INSERT CORE_LIBS 0
          c++experimental
          c++abi
      )
  endif()

  list(APPEND CORE_LIBS
    stdc++fs
    Xext
    X11
    Xrandr
    Xinerama
    Xxf86vm
    Xcursor
    ${CMAKE_THREAD_LIBS_INIT}
    dl

    )
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      target_compile_options(cereal INTERFACE "-Wno-class-memaccess")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(-Wno-exceptions) # 'functor' has a non-throwing exception specification but can still throw
  endif()

  # add compiler-options for AppleClang to ignore linker warnings from assimp on macOS
  if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # clang does not work well with gcc-stdlib, so this is necessary o_0
    add_compile_options(-stdlib=libc++ -isystem /usr/include/c++/v1)
    add_compile_options(-fcolor-diagnostics)
    #add_compile_options(-std=c++17)
    #set(CORE_LIBS "c++experimental c++abi ${CORE_LIBS}")
    #list(APPEND CORE_LIBS c++experimental c++abi)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
  endif()
#elseif(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /MP /std:c++latest")
#  list(APPEND COMPILE_TIME_DEFS _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX)
#  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
endif()
