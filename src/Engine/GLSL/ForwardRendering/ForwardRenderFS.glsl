#version 430 core
#define MAX_LIGHTS_NUM 4

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;
in vec3 Normal_worldspace;

out vec3 color;
uniform vec3 EyePosition_worldspace;

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
	float Intensity;
};

uniform MaterialProperties Material;
uniform LightSource Lights[MAX_LIGHTS_NUM];
uniform int LightNum;

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

vec3 Shading(struct LightSource Light, vec3 n)
{
	float distance = length(Light.Position - Position_worldspace);

	vec3 l = normalize(Light.Position - Position_worldspace);

	vec3 E = normalize(EyePosition_worldspace + EyeDirection_worldspace);
	vec3 H = normalize(l + E);

	float cos_theta = clamp(dot(n,l),0,1);
	float cos_alpha = clamp(dot(H,n),0,1);

	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2,0.2,0.2);

	vec3 MaterialDiffuseColor = DiffuseFactorSelection() * Light.Color * Light.Intensity * cos_theta / distance ;
	vec3 MaterialSpecularColor = Material.Ks * Light.Color * Light.Intensity * pow(cos_alpha, Material.Shiniess) / distance ;

	color = MaterialAmbientColor * MaterialDiffuseColor +
			MaterialSpecularColor +
			MaterialDiffuseColor;

	return color;
}

void main()
{
	vec3 n = normalize( Normal_worldspace );
	vec3 OutColor;

	for(int LightIndex = 0; LightIndex < LightNum; ++LightIndex)
	{
		OutColor += Shading(Lights[LightIndex], n);
	}

	color = OutColor;
}