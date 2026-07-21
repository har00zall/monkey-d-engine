cmake_minimum_required(VERSION 3.24)
project(MonkeyDEngine VERSION 1.0.0 LANGUAGES CXX)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are restricted. Run cmake in a separate build/directory")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

message(STATUS "Downloading dependencies")
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY  "https://github.com/libsdl-org/SDL.git"
    GIT_TAG         "release-3.4.12"
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git
    GIT_TAG         1.0.3
)
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY  https://github.com/syoyo/tinygltf.git
    GIT_TAG         v3.0.0
)

set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BUILD_GAMES OFF CACHE INTERNAL "")
set(WITH_PIC ON CACHE INTERNAL "")

FetchContent_MakeAvailable(SDL3 glm tinygltf)

add_subdirectory(engine)
add_subdirectory(sandbox)
