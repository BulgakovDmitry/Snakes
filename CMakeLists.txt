cmake_minimum_required(VERSION 3.16)

project(Snakes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(Snakes
    src/main.cpp
    src/view_ascii.cpp
    src/view_graphics.cpp
)

target_include_directories(Snakes PRIVATE include)

target_link_libraries(Snakes 
    sfml-graphics 
    sfml-window 
    sfml-system
)
