add_executable(myExe)

target_sources(myExe
    PRIVATE
        src/main.cpp
        )


target_link_libraries(myExe PRIVATE assimp)
