#version 430 core

// Only receive vertex pos
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 depthVP;
uniform mat4 M;

out vec3 Position_worldspace;

void main()
{
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	gl_Position = depthVP * M * vec4(vertexPosition_modelspace,1);
}