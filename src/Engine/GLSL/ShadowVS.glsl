#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 EyeDirection_worldspace;
out vec3 LightDirection_worldspace;
out vec3 Normal_worldspace;
out vec4 ShadowCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;
uniform vec3 EyePosition_worldspace;
uniform float LightPower;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;

uniform mat4 DepthBiasMVP;
uniform sampler2D TexSampler;

mat4 MVP = P * V * M ;

void main()
{

	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace,1);

	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	vec3 vertexPosition_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	EyeDirection_worldspace = vec3(0,0,0) - vertexPosition_worldspace;

	LightDirection_worldspace = LightPosition_worldspace + EyeDirection_worldspace;

	Normal_worldspace = mat3(transpose(inverse(M))) * vertexNormal_modelspace;

	UV = vertexUV;

}