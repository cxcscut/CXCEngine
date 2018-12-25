#version 430 core
#define MAX_LIGHTS_NUM 4

in vec2 UV;
in vec3 Position_worldspace;
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
	vec3 TargetPos;
	float CutOffAngle;
	float Intensity;
};

uniform MaterialProperties Material;
uniform LightSource OmniLights[MAX_LIGHTS_NUM];
uniform LightSource DirectionalLights[MAX_LIGHTS_NUM];
uniform LightSource SpotLights[MAX_LIGHTS_NUM];
uniform int OmniLightNum;
uniform int DirectionalLightNum;
uniform int SpotLightNum;

subroutine vec3 GetDiffuseFactor();
subroutine float AttenuationType(float Distance);

layout (index = 0) subroutine (AttenuationType) float None(float Distance)
{
	return 1.0f;
}

subroutine (AttenuationType) float Linear(float Distance)
{
	return 1 / Distance;
}

subroutine (AttenuationType) float Quadratic(float Distance)
{
	return 1 / pow(Distance, 2);
}

subroutine (AttenuationType) float Cubic(float Distance)
{
	return 1 / pow(Distance, 3);
}

subroutine (GetDiffuseFactor) vec3 TextureDiffuse()
{
	return texture(Material.TexSampler, UV).rgb;
}

subroutine (GetDiffuseFactor) vec3 NonTextureDiffuse()
{
	return Material.Kd;
}

subroutine uniform GetDiffuseFactor DiffuseFactorSelection;
subroutine uniform AttenuationType SpotLightAttenuations[MAX_LIGHTS_NUM];
subroutine uniform AttenuationType OmniLightAttenuations[MAX_LIGHTS_NUM];
subroutine uniform AttenuationType DirectionalLightAttenuations[MAX_LIGHTS_NUM];

vec3 OmniLightFragShading(vec3 n)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0,0,0);
	vec3 E = normalize(EyePosition_worldspace- Position_worldspace);

	for(int LightIndex = 0; LightIndex < OmniLightNum; ++LightIndex)
	{
		float distance = length(OmniLights[LightIndex].Position - Position_worldspace);
		vec3 l = normalize(OmniLights[LightIndex].Position - Position_worldspace);
		vec3 H = normalize(l + E);

		float cos_theta = clamp(dot(n, l), 0, 1);
		float cos_alpha = clamp(dot(H, n), 0, 1);

		vec3 MaterialAmbientColor = Material.Ka * vec3(0.2,0.2,0.2);

		vec3 MaterialDiffuseColor = DiffuseFactorSelection() * OmniLights[LightIndex].Color * OmniLights[LightIndex].Intensity * cos_theta * OmniLightAttenuations[LightIndex](distance);
		vec3 MaterialSpecularColor = Material.Ks * OmniLights[LightIndex].Color * OmniLights[LightIndex].Intensity * pow(cos_alpha, Material.Shiniess) * OmniLightAttenuations[LightIndex](distance) ;
	
		FinalColor += MaterialAmbientColor * MaterialDiffuseColor +
				MaterialSpecularColor +
				MaterialDiffuseColor;
	}

	return FinalColor;
}

vec3 DirectionalLightFragShading(vec3 n)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0,0,0);
	vec3 E = normalize(EyePosition_worldspace- Position_worldspace);

	for(int LightIndex = 0; LightIndex < OmniLightNum; ++LightIndex)
	{
		float distance = length(DirectionalLights[LightIndex].Position - Position_worldspace);
		vec3 l = normalize(DirectionalLights[LightIndex].Position - Position_worldspace);
		vec3 H = normalize(l + E);

		float cos_theta = clamp(dot(n, l), 0, 1);
		float cos_alpha = clamp(dot(H, n), 0, 1);

		vec3 MaterialAmbientColor = Material.Ka * vec3(0.2,0.2,0.2);

		vec3 MaterialDiffuseColor = DiffuseFactorSelection() *DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * cos_theta * DirectionalLightAttenuations[LightIndex](distance);
		vec3 MaterialSpecularColor = Material.Ks * DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * pow(cos_alpha, Material.Shiniess) * DirectionalLightAttenuations[LightIndex](distance) ;
	
		FinalColor += MaterialAmbientColor * MaterialDiffuseColor +
				MaterialSpecularColor +
				MaterialDiffuseColor;
	}

	return FinalColor;
}

vec3 SpotLightFragShading(vec3 n)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0,0,0);

	vec3 EyeDirection = normalize(EyePosition_worldspace- Position_worldspace);

	for(int LightIndex = 0; LightIndex < SpotLightNum; ++LightIndex)
	{
		float LightDistance = length(SpotLights[LightIndex].Position - Position_worldspace);
		vec3 LightDirection = normalize(SpotLights[LightIndex].Position - SpotLights[LightIndex].TargetPos);
		vec3 HalfVector = normalize(LightDirection + EyeDirection);

		vec3 LightToVertexDir = normalize(Position_worldspace - SpotLights[LightIndex].Position);

		float Visibility;
		if(dot(LightToVertexDir, -LightDirection) >= cos(SpotLights[LightIndex].CutOffAngle * 0.5f))
		{
 			Visibility = 1.0f;
		}
		else
		{
			Visibility = 0.0f;
		}

		float CosTheta = clamp(dot(n, LightDirection), 0, 1);
		float CosAlpha = clamp(dot(HalfVector, n), 0, 1);

		MaterialDiffuseColor = Visibility * 0.1 * DiffuseFactorSelection() * SpotLights[LightIndex].Color * SpotLights[LightIndex].Intensity * CosTheta * SpotLightAttenuations[LightIndex](LightDistance);
		MaterialSpecularColor = Visibility * 0.1 * Material.Ks * SpotLights[LightIndex].Color * SpotLights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) * SpotLightAttenuations[LightIndex](LightDistance);
		
		FinalColor += MaterialAmbientColor  +
				MaterialDiffuseColor +
				MaterialSpecularColor;
	}

	return FinalColor;
}

void main()
{
	vec3 n = normalize( Normal_worldspace );
	
	color = OmniLightFragShading(n) +
			DirectionalLightFragShading(n) +
			SpotLightFragShading(n);
}