#version 430 core
#extension GL_NV_shadow_samplers_cube : enable

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;
in vec3 Normal_worldspace;
in vec4 ShadowCoord;

uniform samplerCube shadowmapCube;

out vec3 color;
uniform float LightPower;
uniform vec3 LightPosition_worldspace;
uniform vec3 EyePosition_worldspace;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform mat4 DepthBiasMVP;
uniform float test;

vec2 poissonDisk[16];
float random(vec3 seed, int i);
float VectorToDepthValue(vec3 Vec);

void main()
{
	int shineness = 32;

	float distance = length(LightPosition_worldspace - Position_worldspace);

	vec3 LightColor = vec3(1,1,1);

	vec3 n = normalize( Normal_worldspace );
	vec3 l = normalize( LightDirection_worldspace );

	//vec3 R = reflect(-l,n);
	vec3 E = normalize(EyePosition_worldspace + EyeDirection_worldspace);
	vec3 H = normalize(l + E);

	float cos_theta = clamp(dot(n,l),0,1);
	float cos_alpha = clamp(dot(H,n),0,1);

	float Bias = 0;
	float visibility = 1.0;

	vec3 LightDir = Position_worldspace - LightPosition_worldspace;

	float testDepth = VectorToDepthValue(LightPosition_worldspace - Position_worldspace);

	// 4x sampling
	for (int i=0;i<4;i++)
	{ 

		int index = int(16.0*random(gl_FragCoord.xyy, i))%16;

		if ( textureCube( shadowmapCube, LightDir).z < testDepth - Bias)
		{
			visibility -= 0.2;
		}
	}

	visibility = clamp(visibility, 0.0f, 1.0f);

	vec3 MaterialDiffuseColor = visibility * Kd * LightColor * LightPower * cos_theta / distance;
	vec3 MaterialAmbientColor = Ka * vec3(0.2,0.2,0.2);
	vec3 MaterialSpecularColor = visibility * Ks * LightColor * LightPower * pow(cos_alpha, shineness)  / distance;

	color = MaterialAmbientColor * MaterialDiffuseColor + 
			MaterialDiffuseColor +
			MaterialSpecularColor;
}