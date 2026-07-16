file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp" "src/**/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "include/**/*.h" "include/*.hpp")

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(
    Engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
