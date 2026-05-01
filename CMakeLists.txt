cmake_minimum_required(VERSION 3.28)
project (GaUIL VERSION 0.0.1)
include(FetchContent)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)


set(COMPILE_DEV ON) 
set(SFML_STATIC ON)


FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_Declare(
    SimpleSS
    GIT_REPOSITORY https://github.com/jit-zs/SimpleSS.git
    GIT_TAG v0.0.3
)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
  FetchContent_Populate(stb)
endif()
FetchContent_MakeAvailable(SimpleSS)



add_library(${PROJECT_NAME} STATIC 
    src/Draw.cpp
    src/GaUIL_C.cpp
    src/GUI.cpp
    src/Impls.cpp
    src/NineSlice.cpp
    src/Style.cpp
    src/Text.cpp
    )
target_link_libraries(${PROJECT_NAME} PRIVATE SimpleSS )
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC include)

if (${COMPILE_DEV})
    FetchContent_MakeAvailable(sfml raylib)
    add_executable(dev dev/main.cpp)
    target_link_libraries(dev PRIVATE
        SFML::System
        SFML::Window
        SFML::Graphics
        SFML::Network
        SFML::Audio
        ${PROJECT_NAME}
        )
    add_executable(dev_c dev/main.c)
    target_link_libraries(dev_c PRIVATE
        ${PROJECT_NAME}
        raylib
        )
endif()