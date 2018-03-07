#version 430 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Normal_cameraspace;

uniform sampler2D Sampler;


out vec3 color;
uniform vec3 LightPosition_worldspace;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform int isUseTex;

void main()
{
	
	int shineness = 32;

	float distance = length(LightPosition_worldspace - Position_worldspace);

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 200.0f;

	vec3 H = normalize(LightDirection_cameraspace + EyeDirection_cameraspace);
	float cos_theta = clamp(dot(normalize(Normal_cameraspace),normalize(LightDirection_cameraspace)),0,1);
	float cos_alpha = clamp(dot(normalize(Normal_cameraspace),H),0,1);

	vec3 MaterialAmbientColor = Ka * vec3(0.3,0.3,0.3);
	vec3 MaterialDiffuseColor;

	if(isUseTex > 0)
		MaterialDiffuseColor = LightColor * LightPower * cos_theta * texture2D(Sampler,UV) / distance;
	else
		MaterialDiffuseColor = Kd * LightColor * LightPower *cos_theta / (distance * distance);

	vec3 MaterialSpecularColor = Ks * LightColor * LightPower * pow(cos_alpha,shineness) / distance;

	color = MaterialAmbientColor + MaterialSpecularColor + MaterialDiffuseColor;

}
