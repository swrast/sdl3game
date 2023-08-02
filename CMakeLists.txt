cmake_minimum_required(VERSION 3.16)

project(sdl3game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL3_ROOT_DIR "external/SDL")
set(SDL3_IMAGE_ROOT_DIR "external/SDL_image")
set(ENTT_ROOT_DIR "external/entt")

add_subdirectory("${SDL3_ROOT_DIR}")
add_subdirectory("${SDL3_IMAGE_ROOT_DIR}")
add_subdirectory("${ENTT_ROOT_DIR}")

set(SOURCES
    src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 SDL3_image::SDL3_image EnTT::EnTT)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
