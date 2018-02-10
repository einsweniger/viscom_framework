include(cmake/BuildConfig.cmake)
set(VISCOM_OPENGL_PROFILE "4.1" CACHE STRING "OpenGL profile version to use." FORCE)
set(CMAKE_GENERATOR Ninja)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VISCOM_CLIENTGUI OFF CACHE BOOL "Use ImGui on clients." FORCE)
set(VISCOM_SYNCINPUT OFF CACHE BOOL "Synchronize input from master to clients." FORCE)
set(VISCOM_CLIENTMOUSECURSOR OFF CACHE BOOL "Show the mouse cursor on clients." FORCE)
set(VISCOM_USE_SGCT OFF CACHE BOOL "Use SGCT for local builds.")
set(TUIO_LIB OFF CACHE BOOL "Use TUIO input library" FORCE)
set(ENV{SGCT_ROOT_DIR} "${PROJECT_SOURCE_DIR}/extern/SGCT-2.7.3-bin/mscv14_x64")

if(${VISCOM_USE_SGCT} AND NOT MSVC)
    add_subdirectory(extern/fwcore/extern/sgct)
    include_directories(BEFORE ${PROJECT_BINARY_DIR}/extern/sgct)
endif()