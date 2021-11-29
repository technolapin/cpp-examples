cmake_minimum_required(VERSION 3.20.0)
project(tuto_assimp)

# adds the bin/ dir (which is convenient)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


add_subdirectory(dependencies INTERFACES/dependencies)
add_subdirectory(myCode INTERFACES/myCode)
