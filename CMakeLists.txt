# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project Name
project(SynthBox)

# Specify C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find PortAudio using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)

if(PORTAUDIO_FOUND)
    message(STATUS "Found PortAudio: ${PORTAUDIO_LIBRARIES}")
    include_directories(${PORTAUDIO_INCLUDE_DIRS})
    link_directories(${PORTAUDIO_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "PortAudio not found. Please install PortAudio and try again.")
endif()

# Specify the paths to RtMidi headers and libraries
include_directories("/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

# Add source files
file(GLOB SOURCES src/*.cpp)

# Define the executable
add_executable(synthbox ${SOURCES})

# Link PortAudio and RtMidi libraries
target_link_libraries(synthbox ${PORTAUDIO_LIBRARIES} rtmidi)