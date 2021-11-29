cmake_minimum_required(VERSION 3.20.0)
project(find_dependencies)

# finds assimp if it is installed by searching for
# AssimpConfig.cmake somewhere in your system
# This config file is imported in your system when you install assimp
find_package(assimp CONFIG REQUIRED)
# if it is not installed, use fetch-content or include assimp's source as a subdirectory of dependencies (very unpractical)
