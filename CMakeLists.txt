cmake_minimum_required(VERSION 3.0.2)

project(essynth)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories( src/core src/gui src/modules src/io)

# Add the GUI library
add_subdirectory(src/gui)

# Compile the main engine as a separate library to avoid adding
# -fPIC to the final compilation flags due to Qt needs
add_library(essynthcore
    src/io/ESIoPortAudio.cpp
    src/io/ESIoPortMidi.cpp
    src/core/ESAudioInterface.cpp
    src/core/ESMidiInterface.cpp
    module_test.cpp
    )

# Main application
add_executable(essynth main.cpp)
target_link_libraries (essynth essynthgui essynthcore portaudio portmidi)
