cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(CartaProject CXX)

# set version
set(CARTAPROJECT_MAJOR_VERSION 0)
set(CARTAPROJECT_MINOR_VERSION 9)
set(CARTAPROJECT_PATCH_VERSION 0)
set(CARTAPROJECT_VERSION ${CARTAPROJECT_MAJOR_VERSION}.${CARTAPROJECT_MINOR_VERSION}.${CARTAPROJECT_PATCH_VERSION})
message(STATUS "CartaProject version " ${CARTAPROJECT_VERSION})

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
message(STATUS "CartaProject Build mode=" ${BUILDMODE})
if(${BUILDMODE} STREQUAL "debug")
	add_definitions(-DCARTA_RUNTIME_CHECKS=0)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
	message(STATUS "-DCARTA_RUNTIME_CHECKS=0")
else()
	add_definitions(-DCARTA_RUNTIME_CHECKS=1)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
	message(STATUS "-DCARTA_RUNTIME_CHECKS=1")

endif()

# check prefix
set(PREFIX "" CACHE STRING "default path is '/usr/local/lib/'")
if("${PREFIX}" STREQUAL "")
	# do not thing
else()
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
endif()
message(STATUS "install path=" ${CMAKE_INSTALL_PREFIX})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include header files
include_directories(
#	${PROJECT_SOURCE_DIR}/../ 
	${PROJECT_SOURCE_DIR}
	)

# -pipe  -O2 -g -Wall -W -std=c++0x -D_REENTRANT --DCARTALIB_LIBRARY

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include path='${dir}'")
endforeach()

add_subdirectory(CartaLib)
add_subdirectory(core)
add_subdirectory(desktop)

# Create shared library

# install library

# for debug using make VERBOSE=1
# or set(CMAKE_VERBOSE_MAKEFILE on)

#install(DIRECTORY include/rapidjson
#    DESTINATION "${INCLUDE_INSTALL_DIR}"
#    COMPONENT dev)

