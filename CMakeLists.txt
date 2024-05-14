cmake_minimum_required(VERSION 3.28)
project(ITP)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.5.1)
FETCHCONTENT_MAKEAVAILABLE(SFML)

set(CMAKE_CXX_STANDARD 17)

add_executable(ITP main.cpp
        headers/game.h
        sources/game.cpp
        headers/ball.h
        sources/ball.cpp)

target_link_libraries(ITP sfml-graphics)