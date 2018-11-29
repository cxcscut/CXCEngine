#version 430 core
#define MAX_LIGHT_NUM 4
#extension GL_NV_shadow_samplers_cube : enable

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;
in vec3 Normal_worldspace;
in vec4 ShadowCoord;

uniform sampler2D TexSampler;
uniform samplerCube shadowmapCube[MAX_LIGHT_NUM];

out vec3 color;
uniform float LightPower;
uniform vec3 LightPosition_worldspace;
uniform vec3 EyePosition_worldspace;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ka;
uniform float Shiniess;
uniform mat4 DepthBiasMVP;

// Poisson sampling
vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float VectorToDepthValue(vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 10000.0;
    const float n = 0.1;
    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

struct LightSource
{
	vec3 Position;
	vec3 Color;
	float Intensity;
}

uniform LightSource[MAX_LIGHT_NUM];
uniform int LightNum;

vec3 Shading(struct LightSource Light, vec3 n)
{
	float distance = length(Light.Position - Position_worldspace);

	vec3 l = normalize( Light.Position - Position_worldspace );

	vec3 E = normalize(EyePosition_worldspace + EyeDirection_worldspace);
	vec3 H = normalize(l + E);

	float cos_theta = clamp(dot(n,l),0,1);
	float cos_alpha = clamp(dot(H,n),0,1);

	float Bias = 0.0000001;
	float visibility = 1.0;

	float testDepth = VectorToDepthValue(Light.Position - Position_worldspace);

	// 4x sampling
	for (int i=0;i<4;i++)
	{ 
		int index = int(16.0*random(gl_FragCoord.xyy, i))%16;

		if ( textureCube( shadowmapCube, -l).z < testDepth - Bias)
		{
			visibility -= 0.2;
		}			
	}

	visibility = clamp(visibility, 0.0f, 1.0f);

	vec3 MaterialDiffuseColor = visibility * texture(TexSampler, UV).rgb * Light.Color * Light.Intensity * cos_theta / distance;
	vec3 MaterialAmbientColor = Ka * vec3(0.2,0.2,0.2);
	vec3 MaterialSpecularColor = visibility * Ks * Light.Color * Light.Intensity  * pow(cos_alpha, Shiniess)  / distance;

	RetColor = MaterialAmbientColor * MaterialDiffuseColor + 
			MaterialDiffuseColor +
			MaterialSpecularColor;

	return RetColor;
}

void main()
{
	vec3 n = normalize( Normal_worldspace );

	vec3 OutColor;
	for(int LightIndex; LightIndex < LightNum; ++LightIndex)
	{
		OutColor += Shading(Lights[LightIndex], n);
	}

	color = OutColor;
}