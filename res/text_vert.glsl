#version 330

layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec2 inUV;

out vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec4 color;

void main()
{
	uv = inUV;
	gl_Position = projectionMatrix * modelMatrix * vec4(inVertex.xy, 0.0, 1.0);

}
