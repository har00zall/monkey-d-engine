file(GLOB_RECURSE SANDBOX_SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB_RECURSE SANDBOX_HEADERS "include/*.h" "include/*.hpp")

add_executable(Sandbox ${SANDBOX_SOURCES} ${SANDBOX_HEADERS})

# Custom functions
# Shader Compilation function
find_program(GLSL_VALIDATOR glslc HINTS /usr/bin /usr/local/bin ${VULKAN_SDK}/Bin ${VULKAN_SDK_PATH}/Bin32)
if(NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "glslc not found! Make sure Vulkan SDK is installed and glslc is in PATH")
endif()
function(add_shader TARGET SHADER SHADER_TYPE)
    # Get shader name without extension
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    get_filename_component(SHADER_EXT ${SHADER} EXT)

    string(SUBSTRING ${SHADER_EXT} 1 -1 SHADER_EXT_NO_DOT)

    set(SHADER_OUTPUT ${CMAKE_SOURCE_DIR}/assets/shaders/${SHADER_NAME}.${SHADER_EXT_NO_DOT}.spv)

    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${GLSL_VALIDATOR} -fshader-stage=${SHADER_TYPE} ${CMAKE_SOURCE_DIR}/assets/shaders/${SHADER} -o ${SHADER_OUTPUT}
        DEPENDS ${CMAKE_SOURCE_DIR}/assets/shaders/${SHADER}
        COMMENT "Compiling shader ${SHADER} (${SHADER_TYPE}) to SPIR-V"
    )

    target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT})
endfunction()

#run custom functions

add_shader(Sandbox base.vert vertex)
add_shader(Sandbox base.frag fragment)

add_custom_command(
    TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:Sandbox>/assets"
    COMMENT "Copying assets to build directory..."
)

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
