cmake_minimum_required(VERSION 3.28)
project (GaUIL VERSION 0.0.1)
include(FetchContent)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)


set(COMPILE_DEV OFF "Compile dev application") # Should be set to OFF if you only want the library. Will probably fail.
set(SFML_STATIC ON)


FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)
FetchContent_Declare(
    SimpleSS
    GIT_REPOSITORY https://github.com/jit-zs/SimpleSS.git
    GIT_TAG v0.0.3
)

FetchContent_MakeAvailable(sfml SimpleSS)



add_library(${PROJECT_NAME} STATIC 
    src/Draw.cpp
    src/GUI.cpp
    src/Style.cpp
    )
target_link_libraries(${PROJECT_NAME} PRIVATE SimpleSS)
target_include_directories(${PROJECT_NAME} PUBLIC include)


add_executable(dev dev/main.cpp)
target_link_libraries(dev PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Network
    SFML::Audio
    ${PROJECT_NAME}
    )