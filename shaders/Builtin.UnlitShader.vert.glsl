#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(set = 0, binding = 0) uniform GlobalUniformObject {
	mat4 view;
	mat4 projection;
	mat4 reserved1;
	mat4 reserved2;
} globalUbo;

layout(set = 1, binding = 0) uniform UnlitUniformObject {
	mat4 model;
	mat4 reserved1;
	mat4 reserved2;
	mat4 reserved3;
} objectUbo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = globalUbo.projection * globalUbo.view * objectUbo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}