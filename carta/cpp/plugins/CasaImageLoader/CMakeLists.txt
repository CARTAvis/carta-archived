cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(CasaImageLoader CXX)

# set version
set(CASAIMGLOADER_MAJOR_VERSION 0)
set(CASAIMGLOADER_MINOR_VERSION 9)
set(CASAIMGLOADER_PATCH_VERSION 0)
set(CASAIMGLOADER_VERSION ${CASAIMGLOADER_MAJOR_VERSION}.${CASAIMGLOADER_MINOR_VERSION}.${CASAIMGLOADER_PATCH_VERSION})
message(STATUS "plugin: CasaImageLoader version " ${CASAIMGLOADER_VERSION})

# find package
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

## find cfitio
message(STATUS "searching cftisio...")
find_path(CFITSIO_RELATIVE_DIR
	NAMES /include/fitsio.h
	PATHS "${THIRD_ABS_PATH}"
	PATH_SUFFIXES cfitsio cfitsio-shared
)

get_filename_component(CFITSIO_ROOT_DIR ${CFITSIO_RELATIVE_DIR} REALPATH CACHE)
set(CFITSIO_INCLUDE_DIR "${CFITSIO_ROOT_DIR}/include")

find_library(CFITSIO_LIBRARY
	NAMES cfitsio
	HINTS "${CFITSIO_ROOT_DIR}"
	PATH_SUFFIXES lib lib64
)
message(STATUS "cfitsio root path=" ${CFITSIO_ROOT_DIR})
message(STATUS "cfitsio include path=" ${CFITSIO_INCLUDE_DIR})
message(STATUS "cfitsio library=" ${CFITSIO_LIBRARY})

## find casacore
message(STATUS "searching casacore...")
find_path(CASACORE_RELATIVE_DIR
	NAMES include/casacore/casa/IO.h
	PATHS "${THIRD_ABS_PATH}"
	PATH_SUFFIXES casa casacore
)
get_filename_component(CASACORE_ROOT_DIR ${CASACORE_RELATIVE_DIR} REALPATH CACHE)
set(CASACORE_INCLUDE_DIR "${CASACORE_ROOT_DIR}/include")

find_library(CASACORE_LIBRARY
	NAMES casa_casa casa_coordinates casa_image
	PATHS "${THIRD_ABS_PATH}/casacore/lib"
)
message(STATUS "casacore root path=" ${CASACORE_ROOT_DIR})
message(STATUS "casacore include path=" ${CASACORE_INCLUDE_DIR})
message(STATUS "casacore library=" ${CASACORE_LIBRARY})

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
message(STATUS "CartaLibrary Build mode=" ${BUILDMODE})
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
	${PROJECT_SOURCE_DIR}/../../
	${PROJECT_SOURCE_DIR}
	${CFITSIO_INCLUDE_DIR}
	${CASACORE_ROOT_DIR}/include/
	)

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
target_link_libraries(${PROJECT_NAME} ${CASACORE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${CFITSIO_INCLUDE_DIR}/../lib/libcfitsio.so)
target_link_libraries(${PROJECT_NAME} ${CASACORE_ROOT_DIR}/lib/libcasa_coordinates.so)
target_link_libraries(${PROJECT_NAME} ${CASACORE_ROOT_DIR}/lib/libcasa_images.so)
target_link_libraries(${PROJECT_NAME} ${CASACORE_ROOT_DIR}/lib/libcasa_lattices.so)
target_link_libraries(${PROJECT_NAME} ${CASACORE_ROOT_DIR}/lib/libcasa_measures.so)
target_link_libraries(${PROJECT_NAME} CartaLib)

# install library
install(PROGRAMS ${CMAKE_BINARY_DIR}/cpp/plugins/${PROJECT_NAME}/lib${PROJECT_NAME}.so DESTINATION plugins/${PROJECT_NAME} RENAME libplugin.so)
install(FILES plugin.json DESTINATION  plugins/${PROJECT_NAME})

# for debug using make VERBOSE=1
# or set(CMAKE_VERBOSE_MAKEFILE on)
