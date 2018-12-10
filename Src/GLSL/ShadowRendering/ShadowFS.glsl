#version 430 core

#extension GL_NV_shadow_samplers_cube : enable

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec4 ShadowCoord;

struct MaterialProperties
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shiniess;
	sampler2D TexSampler;
};

struct LightSource
{
	vec3 Position;
	vec3 Color;
	vec3 TargetPos;
	float CutOffAngle;
	float Intensity;
};

out vec3 color;
uniform vec3 LightPosition_worldspace;
uniform vec3 EyePosition_worldspace;
uniform mat4 DepthBiasMVP;
uniform samplerCube shadowmapCube;
uniform MaterialProperties Material;

uniform LightSource OmniLight;
uniform LightSource DirectionalLight;
uniform LightSource SpotLight;

subroutine vec3 GetDiffuseFactor();
subroutine float Atteunation(float LightDistance);
subroutine vec3 LightType();

subroutine (GetDiffuseFactor) vec3 TextureDiffuse()
{
	return texture(Material.TexSampler, UV).rgb;
}

subroutine (GetDiffuseFactor) vec3 NonTextureDiffuse()
{
	return Material.Kd;
}

layout (index = 0) subroutine (Atteunation) float None(float LightDistance)
{
	return 1.0f;
}

subroutine (Atteunation) float Linear(float LightDistance)
{
	return 1 / LightDistance;
}

subroutine (Atteunation) float Quadratic(float LightDistance)
{
	return 1 / pow(LightDistance, 2);
}

subroutine (Atteunation) float Cubic(float LightDistance)
{
	return 1 / pow(LightDistance, 3);
}

subroutine uniform LightType LightTypeSelection;
subroutine uniform Atteunation OmniLightAtteunation;
subroutine uniform Atteunation DirectionalLightAtteunation;
subroutine uniform Atteunation SpotLightAtteunation;
subroutine uniform GetDiffuseFactor DiffuseFactorSelection;

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

subroutine (LightType) vec3 OmniLightFragShading()
{
	float distance = length(OmniLight.Position - Position_worldspace);
	
	vec3 n = normalize( Normal_worldspace );

	vec3 l = normalize( OmniLight.Position - Position_worldspace );

	vec3 E = normalize( EyePosition_worldspace - Position_worldspace);
	vec3 H = normalize(l + E);

	float cos_theta = clamp(dot(n,l),0,1);
	float cos_alpha = clamp(dot(H,n),0,1);

	float Bias = 0;
	float visibility = 1.0;

	float testDepth = VectorToDepthValue(OmniLight.Position - Position_worldspace);

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

	vec3 MaterialDiffuseColor = 0.01 * visibility * DiffuseFactorSelection() * OmniLight.Color * OmniLight.Intensity * cos_theta * OmniLightAtteunation(distance);
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2,0.2,0.2);
	vec3 MaterialSpecularColor = 0.01 * visibility * Material.Ks * OmniLight.Color * OmniLight.Intensity  * pow(cos_alpha, Material.Shiniess) * OmniLightAtteunation(distance);

	vec3 RetColor = MaterialAmbientColor * MaterialDiffuseColor + 
			MaterialDiffuseColor +
			MaterialSpecularColor;

	return RetColor;
}

subroutine (LightType) vec3 DirectionalLightFragShading()
{
	return vec3(0,0,0);
}

subroutine (LightType) vec3 SpotLightFragShading()
{
	return vec3(0,0,0);
}

void main()
{
	color = LightTypeSelection();
}