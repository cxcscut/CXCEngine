#version 430 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

uniform sampler2D Sampler;

out vec3 color;

uniform vec3 LightPosition_worldspace;
uniform float tex_is_used;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;

void main()
{
	
	float distance = length(LightPosition_worldspace - Position_worldspace);

	float LightPower = 1.0f / (distance * distance) ;

	vec3 LightColor = vec3(1,1,1);

	vec3 MaterialDiffuseColor = Kd * LightPower * LightColor * max(dot(normalize(Normal_cameraspace), normalize(LightDirection_cameraspace)),0.0f);
		vec3 MaterialAmbientColor = LightColor * Ka;

		color =  MaterialAmbientColor + MaterialDiffuseColor;	
	
	

}
