cmake_minimum_required(VERSION 3.9)
SET(APP_NAME ViscomFramework)

PROJECT(${APP_NAME})

include(cmake/assimp_config.cmake)
include(cmake/minuseins.cmake)

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(GENERATOR_IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugSlave;DebugOpenGLCalls" CACHE TYPE INTERNAL)
    set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugSlave;DebugOpenGLCalls")
    set(CMAKE_C_FLAGS_DEBUGSLAVE ${CMAKE_C_FLAGS_DEBUG})
    set(CMAKE_CXX_FLAGS_DEBUGSLAVE ${CMAKE_CXX_FLAGS_DEBUG})
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGSLAVE ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGSLAVE ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
    set(COMPILE_DEFINITIONS_DEBUGSLAVE ${COMPILE_DEFINITIONS_DEBUG})

    set(COMPILE_DEFINITIONS_DEBUGOPENGLCALLS ${COMPILE_DEFINITIONS_DEBUG})
    set(CMAKE_CXX_FLAGS_DEBUGOPENGLCALLS ${CMAKE_CXX_FLAGS_DEBUG})
    set(CMAKE_C_FLAGS_DEBUGOPENGLCALLS ${CMAKE_C_FLAGS_DEBUG})
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGOPENGLCALLS ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGOPENGLCALLS ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
endif()


include(${PROJECT_SOURCE_DIR}/extern/fwcore/CMake_core.cmake)
include(${PROJECT_SOURCE_DIR}/extern/fwenh/CMake_enh.cmake)

# enable drop-down in cmake-gui, taken from https://blog.bethcodes.com/cmake-tips-tricks-drop-down-list
# possible to populate from data/*
set(VISCOM_CONFIG_NAME "single" CACHE STRING "Name/directory of the configuration files to be used (see data dir).")
set_property(CACHE VISCOM_CONFIG_NAME PROPERTY STRINGS single quad localms)

set(VISCOM_VIRTUAL_SCREEN_X 1920 CACHE INTEGER "Virtual screen size in x direction.")
set(VISCOM_VIRTUAL_SCREEN_Y 1080 CACHE INTEGER "Virtual screen size in y direction.")

file(GLOB_RECURSE CFG_FILES ${PROJECT_SOURCE_DIR}/config/*.*)
file(GLOB_RECURSE DATA_FILES ${PROJECT_SOURCE_DIR}/data/*.*)
file(GLOB_RECURSE SHADER_FILES ${PROJECT_SOURCE_DIR}/resources/shader/*.*)
file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/app/*.h
    ${PROJECT_SOURCE_DIR}/src/app/*.cpp)
file(GLOB SRC_FILES_ROOT
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(APPEND SRC_FILES ${SRC_FILES_ROOT})
source_group("shader" FILES ${SHADER_FILES})

foreach(f ${SRC_FILES})
    file(RELATIVE_PATH SRCGR ${PROJECT_SOURCE_DIR} ${f})
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

add_executable(${APP_NAME} ${SRC_FILES} ${SRC_FILES_CORE} ${SRC_FILES_ENH} ${SHADER_FILES} ${SHADER_FILES_CORE} ${SHADER_FILES_ENH} ${EXTERN_SOURCES_CORE})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${VISCOM_APP_NAME})
set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${APP_NAME} PRIVATE ${CORE_INCLUDE_DIRS} ${ENH_INCLUDE_DIRS})
target_link_libraries(${APP_NAME} ${CORE_LIBS} ${ENH_LIBS})
target_compile_definitions(${APP_NAME} PRIVATE ${COMPILE_TIME_DEFS})

set(VISCOM_CONFIG_BASE_DIR "../")
set(VISCOM_CONFIG_PROGRAM_PROPERTIES "../config/${VISCOM_CONFIG_NAME}/propertiesPrecompute.xml")
set(VISCOM_CONFIG_SGCT_CONFIG "../data/${VISCOM_CONFIG_NAME}/${VISCOM_CONFIG_NAME}.xml")
set(VISCOM_CONFIG_PROJECTOR_DATA "../data/${VISCOM_CONFIG_NAME}/ProjectorData.xml")
set(VISCOM_CONFIG_LOCAL "0")
configure_file("framework.cfg" "framework.cfg")
set(VISCOM_CONFIG_LOCAL "1 --slave")
configure_file("framework.cfg" "framework_local_slave.cfg")
set(VISCOM_CONFIG_BASE_DIR "./")
set(VISCOM_CONFIG_PROGRAM_PROPERTIES "../../calibration/config/${VISCOM_CONFIG_NAME}/propertiesPrecompute.xml")
set(VISCOM_CONFIG_SGCT_CONFIG "../../calibration/data/${VISCOM_CONFIG_NAME}/${VISCOM_CONFIG_NAME}.xml")
set(VISCOM_CONFIG_PROJECTOR_DATA "../../calibration/data/${VISCOM_CONFIG_NAME}/ProjectorData.xml")
set(VISCOM_CONFIG_LOCAL "-1")
configure_file("framework.cfg" "framework_install.cfg")

if(GENERATOR_IS_MULTI_CONFIG)
    foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        configure_file("RDSettings.cap" "${CONFIG_TYPE}Settings.cap")
    endforeach()
endif()

copy_core_lib_dlls(${APP_NAME})

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME})
install(DIRECTORY resources/ DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME}/resources)
install(FILES ${CMAKE_BINARY_DIR}/framework_install.cfg DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME} RENAME framework.cfg)
