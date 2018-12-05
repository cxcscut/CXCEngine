#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

mat4 MVP = P * V * M ;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPassSelectionVS;

layout (index = 0) subroutine (RenderPassType) void GeometryPassVS()
{
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

	Position = (M * vec4(vertexPosition_modelspace,1)).xyz;

	Normal = mat3(transpose(inverse(M))) * vertexNormal_modelspace;

	TexCoord = vertexUV;
}

layout (index = 1) subroutine (RenderPassType) void LightingPassVS()
{
	gl_Position = vec4(vertexPosition_modelspace, 1.0f);

	TexCoord = vertexUV;
}

void main()
{
	RenderPassSelectionVS();	
}