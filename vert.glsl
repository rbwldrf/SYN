#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec3 fragColor;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	vec3 vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	VertexBuffer vertexBuffer;
} PushConstants;

void main() {
    gl_Position = vec4(PushConstants.vertexBuffer.vertices[gl_VertexIndex], 1.0);
    fragColor = inColor;
}