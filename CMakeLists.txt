cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rpg)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(glm REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} include)

add_library(GAME_LIB
  src/rpg/sprite.cpp
  src/rpg/render.cpp
  src/rpg/player_control.cpp
  src/rpg/ecs.cpp
  src/rpg/systems.cpp
  src/rpg/map.cpp
  src/rpg/movement.cpp
)
# target_include_directories(GAME_LIB)
target_link_libraries(GAME_LIB SDL2::Main SDL2::TTF SDL2::Image ${GLM_LIBRARIES})

add_executable(game src/main.cpp)
target_link_libraries(game GAME_LIB)

