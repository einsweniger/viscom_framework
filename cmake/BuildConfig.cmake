# Build-flags.
if(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wno-unused-function -Wno-unused-parameter -Wextra -Wpedantic -std=c++17")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

  list(APPEND CORE_LIBS
    stdc++fs
    ${X11_X11_LIB}
		${X11_Xrandr_LIB}
		${X11_Xinerama_LIB}
		${X11_Xinput_LIB}
		${X11_Xxf86vm_LIB}
		${X11_Xcursor_LIB}
		${CMAKE_THREAD_LIBS_INIT})

  # add compiler-options for AppleClang to ignore linker warnings from assimp on macOS
  if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)
  endif()
#elseif(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /MP /std:c++latest")
#  list(APPEND COMPILE_TIME_DEFS _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX)
#  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
endif()
