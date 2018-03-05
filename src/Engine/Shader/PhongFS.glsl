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
	
	int shineness = 32;

	float distance = length(LightPosition_worldspace - Position_worldspace);

	vec3 LightColor = vec3(1,1,1);

	vec3 r = normalize(reflect(-LightDirection_cameraspace,Normal_cameraspace));
	float cos_theta = clamp(dot(normalize(Normal_cameraspace),normalize(LightDirection_cameraspace)),0,1);
	float cos_alpha = clamp(dot(r,normalize(EyeDirection_cameraspace)),0,1);

	vec3 MaterialAmbientColor = Ka * LightColor;
	vec3 MaterialDiffuseColor = Kd * LightColor * cos_theta;
	vec3 MaterialSpecularColor = Ks * LightColor *  pow(cos_alpha,shineness);

	color = MaterialAmbientColor + MaterialSpecularColor + MaterialDiffuseColor;

}
