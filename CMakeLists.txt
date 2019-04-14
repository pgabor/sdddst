#
# Main CMakeLists.txt for sdddst project
# Author:
#    Gabor Peterffy (peterffy95 [\] gmail [!] com)
#
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# C support is turned on by default, specifying only CXX turns C off
set(PROJECT_NAME "sdddst")

# Set project name and supported language
project(${PROJECT_NAME} LANGUAGES CXX)

# Search for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
message(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

include(GNUInstallDirs)

# after this point ${PROJECT_SOURCE_DIR} is set and ready to use

# Options
option(BUILD_PYTHON_BINDINGS "Build python interface package" OFF)

# Version number
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 1)

# Set some variables to use later on in this file
set(PROJECT_SOURCE_DECLARATION_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DEFINITION_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set(MAIN_FILE ${PROJECT_SOURCE_DEFINITION_DIRECTORY}/main.cpp)
set(BINDINGS_FILE ${PROJECT_SOURCE_DEFINITION_DIRECTORY}/bindings.cpp)
set(PYTHON_LIB_NAME PySdddstCore)
set(PY_VERSION "3")

set(BOOST_MIN_VERSION "1.40")
set(CORE_LIBRARY ${PROJECT_NAME}Core)

# Set verbosity if build type is Debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(${PROJECT_NAME}_VERBOSE 1)
else()
    set(${PROJECT_NAME}_VERBOSE 0)
endif()


# configure header file to pass CMake settings
# to source code
configure_file (
	"${PROJECT_SOURCE_DIR}/${PROJECT_NAME}CMakeConfig.h.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}CMakeConfig.h"
	)


# Set the CMAKE_PREFIX_PATH for the find_library fonction when using non
# standard install location
IF(CMAKE_INSTALL_PREFIX)
  SET(CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}" ${CMAKE_PREFIX_PATH})
ENDIF(CMAKE_INSTALL_PREFIX)


# Set compiler flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# Taken from: http://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wextra")
endif()
# use -DCMAKE_BUILD_TYPE="Debug|Release" to choose
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_CXX_STANDARD 11) # std: C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON) # is required


# Find required packages
find_package(PythonLibs ${PY_VERSION} REQUIRED)

include_directories(${PYTHON_INCLUDE_DIR})

FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    # with SYSTEM the compiler knows that they aren't our headers thus
    # warnings & co. will be omitted
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    set(BOOST_BASIC_LIBRARIES ${Boost_LIBRARIES})
endif()

set(Boost_FOUND FALSE)

if (BUILD_PYTHON_BINDINGS)
    FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} COMPONENTS system python3)
    if(Boost_FOUND)
        # with SYSTEM the compiler knows that they aren't our headers thus
        # warnings & co. will be omitted
        include_directories(SYSTEM ${Boost_INCLUDE_DIR})
        set(BOOST_PYTHON_LIBRARIES ${Boost_LIBRARIES})
    else()
        set(BUILD_PYTHON_BINDINGS FALSE)
    endif()
endif()

find_package("Umfpack" MODULE REQUIRED)
include_directories(SYSTEM ${UMFPACK_INCLUDES})

find_package(GSL MODULE REQUIRED)
include_directories(SYSTEM ${GSL_INCLUDE_DIRS})

# Set include directory for headers
include_directories(${PROJECT_SOURCE_DECLARATION_DIRECTORY})

# Add binary tree to search path (the CMakeConfig.h will be placed here)
include_directories ("${PROJECT_BINARY_DIR}")

# Add all source files to SOURCES
file(GLOB SOURCES_CXX
    "${PROJECT_SOURCE_DEFINITION_DIRECTORY}/*.cpp"
    "${PROJECT_SOURCE_DEFINITION_DIRECTORY}/Fields/*.cpp"
    "${PROJECT_SOURCE_DEFINITION_DIRECTORY}/StressProtocols/*.cpp")
file(GLOB HEADERS "${PROJECT_SOURCE_DECLARATION_DIRECTORY}/*.h")

# Remove main.cpp from the sources
list(REMOVE_ITEM SOURCES_CXX ${MAIN_FILE})
list(REMOVE_ITEM SOURCES_CXX ${BINDINGS_FILE})

# Create core library
add_library(${PROJECT_NAME}Core ${SOURCES_CXX})

if(BUILD_PYTHON_BINDINGS)
    add_library(${PYTHON_LIB_NAME} SHARED ${BINDINGS_FILE})
    set_target_properties(${PYTHON_LIB_NAME} PROPERTIES PREFIX "")
endif()

target_link_libraries(${PROJECT_NAME}Core LINK_PUBLIC ${BOOST_BASIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Core LINK_PUBLIC ${UMFPACK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Core LINK_PUBLIC ${GSL_LIBRARIES})

# Create python library
if (BUILD_PYTHON_BINDINGS)
    target_link_libraries(${PYTHON_LIB_NAME} ${BOOST_BASIC_LIBRARIES} ${BOOST_PYTHON_LIBRARIES} ${PROJECT_NAME}Core ${PYTHON_LIBRARY})
endif()
# Resulting main executable of compilation
add_executable(${PROJECT_NAME} ${MAIN_FILE})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CORE_LIBRARY})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${BOOST_BAISC_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Core LINK_PUBLIC ${UMFPACK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}Core LINK_PUBLIC ${GSL_LIBRARIES})
