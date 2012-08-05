cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

# Name and version of the project (0.0.0 currently)
project (RTCWCOOP)
set (RTCWCOOP_VERSION_MAJOR 0)
set (RTCWCOOP_VERSION_MINOR 0)
set (RTCWCOOP_VERSION_PATCH 0)

# Build type can be None, Debug, Release, RelWithDebInfo or MinSizeRel
set (CMAKE_BUILD_TYPE Debug)

# Dependencies
find_package(SDL 1.2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL_INCLUDE_DIRS} ${OpenGL_INCLUDE_DIRS})

# This second file in src/ contains the list of source files
include(src/SourceLists.cmake)

# Targets (main binary and libraries)
add_library(cgame ${CGAME_SOURCES})
add_library(qagame ${QAGAME_SOURCES})
add_library(ui ${UI_SOURCES})
include_directories(${CGAME_INCLUDES} ${QAGAME_INCLUDES} ${UI_INCLUDES})

add_executable(wolfsp ${BZZWOLFSP_SOURCES})
add_executable(wolfspded ${SERVER_SOURCES})
include_directories(${BZZWOLFSP_INCLUDES} ${SERVER_INCLUDES})
target_link_libraries (wolfsp ${SDL_LIBRARIES} ${OPENGL_LIBRARIES})

# Other parameters like output name, compiler flags, preprocessor...
set (CMAKE_EXECUTABLE_SUFFIX .i386)
set_target_properties(cgame PROPERTIES COMPILE_FLAGS "-DCGAMEDLL")
set_target_properties(qagame PROPERTIES COMPILE_FLAGS "-DGAMEDLL")
set_target_properties(cgame qagame ui PROPERTIES PREFIX "" SUFFIX "i386.so")

# Build only 32-bit binaries
if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
endif (CMAKE_COMPILER_IS_GNUCC)
if (APPLE)
	set(CMAKE_OSX_ARCHITECTURES "i386")
endif (APPLE)

# Create "main" directory next to the binaries, for the .so and .pk3 files
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/main")
