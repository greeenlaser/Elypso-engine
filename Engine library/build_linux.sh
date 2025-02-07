#!/bin/bash

# Define message variables
prexc="[PROGRAM_EXCEPTION]"
prinf="[PROGRAM_INFO]"
prcln="[PROGRAM_CLEANUP]"
cminf="[CMAKE_INFO]"
cmexc="[CMAKE_EXCEPTION]"
cmsuc="[CMAKE_SUCCESS]"
cpinf="[CPACK_INFO]"
cpexc="[CPACK_EXCEPTION]"
cpsuc="[CPACK_SUCCESS]"

# Define paths
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
buildPath="$script_dir/out/build/x64-release"
sourcePath="$script_dir"

# Initialize number of cores
numCores=$(nproc)

# Function to pause
function pause() {
    if [ "$2" != "skipwait" ]; then
        read -p "Press Enter to exit..."
    fi
}

# Initialize the environment
echo "$cminf Initializing environment for g++ build..."
if ! command -v g++ &> /dev/null; then
    echo "$prexc g++ is not installed or not in PATH. Please install g++."
    pause "$1" "$2"
    exit 1
fi

function copy_to_game_template() {
   if [ -d "$sourcePath/../Game/" ] 
   then
      cp "$buildPath/libElypso engine.a" "$sourcePath/../Game/"
   fi
}

# Build the project
function build() {
    cd "$buildPath" || exit
    echo "$cminf Started build generation using $numCores cores."
    make -j"$numCores"
    if [ $? -ne 0 ]; then
        echo "$cmexc Build failed."
        pause "$1" "$2"
        exit 0
    else
        echo "$cmsuc Build succeeded!"
        pause "$1" "$2"
        exit 0
    fi
}

# Configure CMake
function cmake_configure() {
    if [ -d "$buildPath" ]; then
        echo "$prcln Removing existing build directory."
        rm -rf "$buildPath"
    fi
    mkdir -p "$buildPath"
    cd "$buildPath" || exit

    echo "$cminf Configuring the project with CMake..."
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" "$sourcePath"
    if [ $? -ne 0 ]; then
        echo "$cmexc Configuration failed."
        pause "$1" "$2"
        exit 1
    fi

    build "$1" "$2"
}

# Main logic
if [ -z "$1" ]; then
    set -- build "$2"
fi

case "$1" in
    build)
        if [ ! -d "$buildPath" ]; then
            echo "$prexc Did not find build folder. Running 'Reconfigure CMake'."
            cmake_configure "$1" "$2"
        else
            build "$1" "$2"
        fi
        ;;
    cmake)
        cmake_configure "$1" "$2"
        ;;
    *)
        echo "$prexc Unknown command. Defaulting to 'cmake'."
        cmake_configure "$1" "$2"
        ;;
esac

pause "$1" "$2"
