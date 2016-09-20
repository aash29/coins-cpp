cmake_minimum_required(VERSION 3.5)
project(coins)

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/include/Box2D /home/aash29/cpp/coins-cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -g")
set(SOURCE_FILES main.cpp Test.cpp DebugDraw.cpp RenderGL3.cpp TestEntries.cpp imgui.cpp imgui_draw.cpp imgui_impl_glfw_gl3.cpp curve.cpp graph2.cpp cycles.cpp)

set(SOURCE_DIR /home/aash29/cpp/coins-cpp)

include_directories(${SOURCE_DIR})

add_executable(coins ${SOURCE_FILES} graph2.h)

find_package(GLEW)

find_package(OpenGL)

find_package(Box2D)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

target_link_libraries(coins GLEW; OpenGL; glfw ; Box2D)