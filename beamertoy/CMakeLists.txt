project(beamertoy)

option(OPTION_BUILD_EXAMPLES "" OFF)
option(OPTION_BUILD_TOOLS    "" OFF)
option(OPTION_BUILD_SHARED_LIBS "" ON)
add_subdirectory(extern/glbinding)
include_directories(extern/glbinding/source/glbinding/include)

option(WITH_WERROR "" OFF)
option(SKIP_PORTABILITY_TEST "" ON)
add_subdirectory(extern/cereal)
target_compile_options(cereal INTERFACE "-Wimplicit-fallthrough")
include_directories(extern/cereal/include)

include_directories(../extern/fwcore/extern/sgct/include)
include_directories(include)

add_subdirectory(gui)
add_subdirectory(inspect)


add_executable(beamertoy
        main.cpp
        app/ApplicationNodeImplementation.cpp
        app/CoordinatorNode.cpp
        app/WorkerNode.cpp

        app/ToyPlayer.cpp
        app/IntrospectableFsq.cpp
        app/handlers/ProgramOutputHandler.cpp
        app/builders/ExternalUniformBuilder.cpp
        app/builders/ShaderToySamplerBuilder.cpp)
target_link_libraries(beamertoy PUBLIC VISCOMCore INTERFACE glbinding::glbinding glbinding::glbinding-aux PRIVATE gui inspect)
target_include_directories(beamertoy PRIVATE  . INTERFACE VISCOMCore)
set_property(TARGET beamertoy PROPERTY CXX_STANDARD 17)