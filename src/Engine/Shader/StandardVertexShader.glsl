#version 430 core

#define MAX_SHAPE_NUM 100

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexColor;

out vec2 UV;
out vec3 Color;
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 Normal_cameraspace;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

uniform mat4 T[MAX_SHAPE_NUM];
uniform int indices[MAX_SHAPE_NUM];

int i = 0;

void main(){

	if(i < MAX_SHAPE_NUM && gl_VertexID == indices[i])
		i++;

	mat4 MVP = P * V * M * T[i];

	gl_Position = MVP * vec4(vertexPosition_modelspace,1);
	
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace,1)).xyz;

	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;

	UV = vertexUV;
	
	Color = vertexColor;

}