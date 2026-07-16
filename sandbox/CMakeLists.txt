file(GLOB_RECURSE SANDBOX_SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "include/*.h" "include/*.hpp")

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

target_link_libraries(
    Sandbox PRIVATE Engine
)

target_include_directories(Sandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(
    Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/assets"
)
