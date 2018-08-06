#project(gui)
#set(target gui)

add_library(gui Gui.cpp dialogs/FileSelect.cpp)
target_include_directories(gui PRIVATE VISCOMCore INTERFACE cereal)
target_link_libraries(gui PRIVATE VISCOMCore glbinding::glbinding)
set_property(TARGET gui PROPERTY CXX_STANDARD 17)

get_target_property(INXX gui INTERFACE_INCLUDE_DIRECTORIES)
foreach(f ${INXX})
    message(${f})
endforeach()