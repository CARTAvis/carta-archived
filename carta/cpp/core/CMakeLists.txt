cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(core CXX)

# set version
set(CARTACORE_MAJOR_VERSION 0)
set(CARTACORE_MINOR_VERSION 9)
set(CARTACORE_PATCH_VERSION 0)
set(CARTACORE_VERSION ${CARTACORE_MAJOR_VERSION}.${CARTACORE_MINOR_VERSION}.${CARTACORE_PATCH_VERSION})
message(STATUS "CartaCore version " ${CARTACORE_VERSION})

# find package
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)

## find qwt
message(STATUS "searching qwt...")
find_path(QWT_INCLUDE_DIR
	NAMES qwt.h
	PATHS "${PROJECT_SOURCE_DIR}/../../../ThirdParty/qwt/include"
  PATH_SUFFIXES qwt-qt4 qwt qwt5 qwt6
)
message(STATUS "qwt include path=" ${QWT_INCLUDE_DIR})

find_library(QWT_LIBRARY
	NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
	PATHS "${PROJECT_SOURCE_DIR}/../../../ThirdParty/qwt/lib"
)
message(STATUS "qwt library=" ${QWT_LIBRARY})

## find rapidrson
message(STATUS "searching rapidjson...")
find_path(RAPIDJSON_INCLUDE_DIR
	NAMES rapidjson/rapidjson.h
	PATHS "${PROJECT_SOURCE_DIR}/../../../ThirdParty/rapidjson/include/"
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
	add_definitions(-DCARTA_RUNTIME_CHECKS=1)
	message(STATUS "-DCARTA_RUNTIME_CHECKS=1")

endif()
message(STATUS "-CXX_FLAGS= " ${CMAKE_CXX_FLAGS})

# check prefix
set(PREFIX "" CACHE STRING "default path is '/usr/local/lib/'")
if("${PREFIX}" STREQUAL "")
	# do not thing
else()
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
endif()
message(STATUS "install path=" ${CMAKE_INSTALL_PREFIX})

# check file
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h)
#message(STATUS ${CartaLib_src})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include header files
include_directories(
	${PROJECT_SOURCE_DIR}/../ 
	${PROJECT_SOURCE_DIR}
	${QWT_INCLUDE_DIR}
	${RAPIDJSON_INCLUDE_DIR}
	)

# -pipe  -O2 -g -Wall -W -std=c++0x -D_REENTRANT --DCARTALIB_LIBRARY

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include path='${dir}'")
endforeach()

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SRC})
#target_include_directories:
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Network)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${QWT_LIBRARY})
target_link_libraries(${PROJECT_NAME} CartaLib)

# install library
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

# for debug using make VERBOSE=1
# or set(CMAKE_VERBOSE_MAKEFILE on)
