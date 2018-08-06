cmake_minimum_required(VERSION 3.9)
SET(APP_NAME ProjectorFramework)

PROJECT(${APP_NAME})

add_subdirectory(extern/fwcore)

set(VISCOM_CONFIG_NAME "single" CACHE STRING "Name/directory of the configuration files to be used.")
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
find_package(Threads REQUIRED)

add_executable(${APP_NAME} ${SRC_FILES} ${SHADER_FILES})
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ${VISCOM_APP_NAME})
set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${APP_NAME} PRIVATE src)
target_link_libraries(${APP_NAME} dl X11 Xrandr Xcursor Xinerama Xxf86vm ${CMAKE_THREAD_LIBS_INIT} VISCOMCore)

set(VISCOM_CONFIG_BASE_DIR "../")
set(VISCOM_CONFIG_PROGRAM_PROPERTIES "../config/${VISCOM_CONFIG_NAME}/propertiesPrecompute.xml")
set(VISCOM_CONFIG_SGCT_CONFIG "../data/${VISCOM_CONFIG_NAME}/${VISCOM_CONFIG_NAME}.xml")
set(VISCOM_CONFIG_PROJECTOR_DATA "../data/${VISCOM_CONFIG_NAME}/ProjectorData.xml")
set(VISCOM_CONFIG_LOCAL "0")
configure_file("framework.cfg" "framework.cfg")
set(VISCOM_CONFIG_LOCAL "1 --slave")
configure_file("framework.cfg" "framework_local_worker.cfg")
set(VISCOM_CONFIG_BASE_DIR "./")
set(VISCOM_CONFIG_PROGRAM_PROPERTIES "../../calibration/config/${VISCOM_CONFIG_NAME}/propertiesPrecompute.xml")
set(VISCOM_CONFIG_SGCT_CONFIG "../../calibration/data/${VISCOM_CONFIG_NAME}/${VISCOM_CONFIG_NAME}.xml")
set(VISCOM_CONFIG_PROJECTOR_DATA "../../calibration/data/${VISCOM_CONFIG_NAME}/ProjectorData.xml")
set(VISCOM_CONFIG_LOCAL "-1")
configure_file("framework.cfg" "framework_install.cfg")

if(GENERATOR_IS_MULTI_CONFIG)
    foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        if(${CONFIG_TYPE} STREQUAL "DebugWorker")
            set(VISCOM_CMD_LINE "framework_local_worker.cfg")
        else()
            set(VISCOM_CMD_LINE "")
        endif()
        configure_file("RDSettings.cap" "${CONFIG_TYPE}Settings.cap")
    endforeach()
endif()

copy_core_lib_dlls(${APP_NAME})

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME})
install(DIRECTORY resources/ DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME}/resources)
install(FILES ${CMAKE_BINARY_DIR}/framework_install.cfg DESTINATION ${VISCOM_INSTALL_BASE_PATH}/${VISCOM_APP_NAME} RENAME framework.cfg)

add_subdirectory(beamertoy)
