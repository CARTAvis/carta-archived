cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(CartaLib CXX)

# set version
set(CARTALIB_MAJOR_VERSION 0)
set(CARTALIB_MINOR_VERSION 9)
set(CARTALIB_PATCH_VERSION 0)
set(CARTALIB_VERSION ${CARTALIB_MAJOR_VERSION}.${CARTALIB_MINOR_VERSION}.${CARTALIB_PATCH_VERSION})
message(STATUS "CartaLibrary version " ${CARTALIB_VERSION})

# find package
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
#get_target_property(QtCore_location Qt5::Core LOCATION)
#message(STATUS "location " ${QtCore_location})

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

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include header files
include_directories(
	${PROJECT_SOURCE_DIR}/../ 
#	${PROJECT_SOURCE_DIR}
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

# install library
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

