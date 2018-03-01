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

	vec3 LightColor = vec3(1,1,1);

	vec3 MaterialDiffuseColor = Color * tex_is_used * (texture( Sampler, UV ).rgb -1.0f) + 1.0f*Color;
	vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	float distance = length(LightPosition_worldspace - Position_worldspace);

	float LightPower = 0.8f * distance * distance;

	vec3 n = normalize(Normal_cameraspace);

	vec3 l = normalize(LightDirection_cameraspace);

	float cosTheta = clamp(dot(n,l),0,1);

	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n);

	float cosAlpha = clamp(dot(E,R),0,1);

	color =  MaterialAmbientColor +
			 MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
			 MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance);

}
