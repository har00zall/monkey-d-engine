file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/**/*.cpp" "${CMAKE_SOURCE_DIR}/vendors/*.cpp" "${CMAKE_SOURCE_DIR}/vendors/*.c" "${CMAKE_SOURCE_DIR}/vendors/*.cc")
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h" "src/**/*.h" "src/*.hpp" "${CMAKE_SOURCE_DIR}/vendors/*.hpp" "${CMAKE_SOURCE_DIR}/vendors/*.h")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(
    Engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(
    Engine PUBLIC
    SDL3::SDL3 glm tinygltf
)

if (MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
