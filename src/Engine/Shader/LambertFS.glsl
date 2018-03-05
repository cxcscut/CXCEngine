#version 430 core

in vec2 UV;
in vec3 Color;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

uniform sampler2D Sampler;

out vec3 color;

uniform vec3 LightPosition_worldspace;
uniform float tex_is_used;

void main()
{
	
	float distance = length(LightPosition_worldspace - Position_worldspace);

	float LightPower = 0.8f / (distance * distance);

	vec3 LightColor = vec3(1,1,1);

	vec3 MaterialDiffuseColor = Color * LightPower * dot(normalize(Normal_cameraspace), normalize(LightDirection_cameraspace));
	vec3 MaterialAmbientColor = vec3(0.5,0.5,0.5) * Color;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	color =  MaterialAmbientColor +
			 MaterialDiffuseColor;

}
