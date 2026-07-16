#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
    mat4 model;
} uniformBufferObject;

void main() {
    mat4 modelMatrix = uniformBufferObject.model;
    fragPos = vec3(modelMatrix * vec4(inPosition, 1.0));

    gl_Position = uniformBufferObject.viewProjection * vec4(fragPos, 1.0);

    fragNormal = inNormal;
}
