project(beamertoy)

add_subdirectory(inspect)

add_executable(beamertoy main.cpp app/ApplicationNodeImplementation.cpp app/CoordinatorNode.cpp app/WorkerNode.cpp)
target_link_libraries(beamertoy PRIVATE VISCOMCore)
target_include_directories(beamertoy PRIVATE ../extern/fwcore/extern/sgct/include .)
set_property(TARGET beamertoy PROPERTY CXX_STANDARD 17)