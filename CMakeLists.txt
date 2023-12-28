cmake_minimum_required(VERSION 3.16)
project(SFMLTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

include_directories(src/headers)

add_executable(main src/main.cpp src/soundManager.cpp src/player.cpp src/spriteSheet.cpp src/tileSet.cpp src/csvParser.cpp)
target_link_libraries(main sfml-graphics sfml-audio)
