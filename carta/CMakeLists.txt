cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(CartaProject CXX)

# set version
set(CARTAPROJECT_MAJOR_VERSION 0)
set(CARTAPROJECT_MINOR_VERSION 9)
set(CARTAPROJECT_PATCH_VERSION 0)
set(CARTAPROJECT_VERSION ${CARTAPROJECT_MAJOR_VERSION}.${CARTAPROJECT_MINOR_VERSION}.${CARTAPROJECT_PATCH_VERSION})
message(STATUS "CartaProject version " ${CARTAPROJECT_VERSION})

# find package
find_package(Qt5Core REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Network REQUIRED)
#find_package(Qt5Widgets REQUIRED)
#find_package(Qt5WebKitWidgets REQUIRED)
#find_package(Qt5Xml REQUIRED)

## find qwt
message(STATUS "searching qwt...")
find_path(QWT_INCLUDE_DIR
	NAMES qwt.h
	PATHS "${PROJECT_SOURCE_DIR}/../ThirdParty/qwt/include/"
  PATH_SUFFIXES qwt-qt4 qwt qwt5 qwt6
)
message(STATUS "qwt include path=" ${QWT_INCLUDE_DIR})

find_library(QWT_LIBRARY
	NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
	PATHS "${PROJECT_SOURCE_DIR}/../ThirdParty/qwt/lib/"
)
message(STATUS "qwt library=" ${QWT_LIBRARY})

## find rapidrson
message(STATUS "searching rapidjson...")
find_path(RAPIDJSON_INCLUDE_DIR
	NAMES rapidjson/rapidjson.h
	PATHS "${PROJECT_SOURCE_DIR}/../ThirdParty/rapidjson/include/"
)
message(STATUS "rapidjson include path=" ${RAPIDJSON_INCLUDE_DIR})

# check -std=c++0x
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

# check debug / release
set(BUILDMODE "release" CACHE STRING "default mode is 'release'")
message(STATUS "-Build mode=" ${BUILDMODE})
if(${BUILDMODE} STREQUAL "debug")
	add_definitions(-DCARTA_RUNTIME_CHECKS=0)
	message(STATUS "-DCARTA_RUNTIME_CHECKS=0")
else()
#	add_definitions(-DCARTA_RUNTIME_CHECKS=1)
	message(STATUS "-DCARTA_RUNTIME_CHECKS=1")

endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")
message(STATUS "-CXX_FLAGS= " ${CMAKE_CXX_FLAGS})

# check prefix
set(PREFIX "" CACHE STRING "default path is '/usr/local/lib/'")
if("${PREFIX}" STREQUAL "")
	# do not thing
else()
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
endif()
message(STATUS "install path=" ${CMAKE_INSTALL_PREFIX})

# check third party library path
get_filename_component(THIRD_ABS_PATH ${PROJECT_SOURCE_DIR}/../ThirdParty REALPATH CACHE)

# write useful files
file(WRITE  ${CMAKE_BINARY_DIR}/carta-vars.sh "ulimit -n 2048\n")
file(APPEND ${CMAKE_BINARY_DIR}/carta-vars.sh "export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:\${LD_LIBRARY_PATH}\n")
file(APPEND ${CMAKE_BINARY_DIR}/carta-vars.sh "export LD_LIBRARY_PATH=${THIRD_ABS_PATH}/casacore/lib:\${LD_LIBRARY_PATH}\n")
file(APPEND ${CMAKE_BINARY_DIR}/carta-vars.sh "export LD_LIBRARY_PATH=${THIRD_ABS_PATH}/cfitsio/lib:\${LD_LIBRARY_PATH}\n")
file(APPEND ${CMAKE_BINARY_DIR}/carta-vars.sh "export LD_LIBRARY_PATH=${THIRD_ABS_PATH}/wcslib/lib:\${LD_LIBRARY_PATH}\n")
file(APPEND ${CMAKE_BINARY_DIR}/carta-vars.sh "export LD_LIBRARY_PATH=${THIRD_ABS_PATH}/qwt/lib:\${LD_LIBRARY_PATH}\n")

file(WRITE  ${CMAKE_BINARY_DIR}/config.json "{\n")
file(APPEND ${CMAKE_BINARY_DIR}/config.json " \"pluginDirs\":[\"${CMAKE_INSTALL_PREFIX}/plugins\"], \n")
file(APPEND ${CMAKE_BINARY_DIR}/config.json " \"qtDecorations\" : \"true\", \n")
file(APPEND ${CMAKE_BINARY_DIR}/config.json " \"disabledPlugins\" : [] \n")
file(APPEND ${CMAKE_BINARY_DIR}/config.json "}\n")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include header files
include_directories(
	${PROJECT_SOURCE_DIR}
	)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include path='${dir}'")
endforeach()

# Compile Carta
add_subdirectory(html5)
add_subdirectory(cpp/CartaLib)
add_subdirectory(cpp/core)
add_subdirectory(cpp/desktop)
add_subdirectory(cpp/plugins/CasaImageLoader)

# install
install(FILES ${CMAKE_BINARY_DIR}/config.json DESTINATION $ENV{HOME}/.cartavis)
install(PROGRAMS ${CMAKE_BINARY_DIR}/carta-vars.sh DESTINATION bin)
