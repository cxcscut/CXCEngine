#version 430

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gDiffuse;
layout(location = 2) out vec3 gNormal;

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 Normal_worldspace;

struct MaterialProperties
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shiniess;
	sampler2D TexSampler;
};

uniform MaterialProperties Material;

subroutine vec3 GetDiffuseFactor();
subroutine (GetDiffuseFactor) vec3 TextureDiffuse()
{
	return texture(Material.TexSampler, UV).rgb;
}

subroutine (GetDiffuseFactor) vec3 NonTextureDiffuse()
{
	return Material.Kd;
}

subroutine uniform GetDiffuseFactor DiffuseFactorSelection;

void main()
{
	
}