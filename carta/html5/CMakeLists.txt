cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(html)

# set version
#set(CARTADESKTOP_MAJOR_VERSION 0)
#set(CARTADESKTOP_MINOR_VERSION 9)
#set(CARTADESKTOP_PATCH_VERSION 0)
#set(CARTADESKTOP_VERSION ${CARTADESKTOP_MAJOR_VERSION}.${CARTADESKTOP_MINOR_VERSION}.${CARTADESKTOP_PATCH_VERSION})
#message(STATUS "CartaDesktop version " ${CARTADESKTOP_VERSION})

# find package

## find qooxdoo
message(STATUS "searching qooxdoo...")
find_path(QD_BIN_DIR
	NAMES generator.py 
	PATHS "${PROJECT_SOURCE_DIR}/../../ThirdParty/qooxdoo-3.5-sdk/tool/bin"
)
message(STATUS "qooxdoo bin path=" ${QD_BIN_DIR})
get_filename_component(QD_ABS_PATH ${PROJECT_SOURCE_DIR}/../../ThirdParty/qooxdoo-3.5-sdk/ REALPATH CACHE)
message(STATUS "qooxdoo bin path=" ${QD_ABS_PATH})

add_custom_target( genJS
    COMMAND cd ${PROJECT_SOURCE_DIR}/common/skel && python ${QD_BIN_DIR}/generator.py source
    COMMAND cd ${PROJECT_SOURCE_DIR}/common/skel && python ${QD_BIN_DIR}/generator.py
)

#add_custom_target(installJS
#    COMMAND cmake -E copy_directory ${PROJECT_SOURCE_DIR} ${CMAKE_PREFIX_PATH}/etc/html5)
#install(DIRECTORY ${QD_BIN_DIR}/../../ DESTINATION etc/html5/common)
#add_dependencies(installJS install )
install(DIRECTORY ${PROJECT_SOURCE_DIR} DESTINATION etc)
#install(DIRECTORY ${QD_ABS_PATH} DESTINATION etc/html5/common/qooxdoo-3.5-sdk)
