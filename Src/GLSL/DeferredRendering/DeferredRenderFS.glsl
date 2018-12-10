#version 430 core
#define DEFERRED_MAX_LIGHTS_NUM 32

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec3 gDiffuseFactor;
layout (location = 3) out vec3 gNormal;

subroutine float AttenuationType(float Distance);
subroutine void RenderPassType();
subroutine vec3 DiffuseModel();

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
	float AreaRadius;
	float CutOffAngle;
	float Intensity;
};

uniform MaterialProperties Material;
uniform LightSource OmniLights[DEFERRED_MAX_LIGHTS_NUM];
uniform LightSource DirectionalLights[DEFERRED_MAX_LIGHTS_NUM];
uniform LightSource SpotLights[DEFERRED_MAX_LIGHTS_NUM];
uniform int OmniLightNum;
uniform int DirectionalLightNum;
uniform int SpotLightNum;
uniform vec3 EyePosition;

subroutine uniform AttenuationType SpotLightAttenuations[DEFERRED_MAX_LIGHTS_NUM];
subroutine uniform AttenuationType OmniLightAttenuations[DEFERRED_MAX_LIGHTS_NUM];
subroutine uniform AttenuationType DirectionalLightAttenuations[DEFERRED_MAX_LIGHTS_NUM];
subroutine uniform RenderPassType RenderPassSelectionFS;
subroutine uniform DiffuseModel DiffuseModelSelection;

uniform sampler2D VertexPositionTex;
uniform sampler2D VertexNormalTex;
uniform sampler2D VertexDiffuseTex;

layout (index = 0) subroutine (AttenuationType) float None(float Distance)
{
	return 1.0f;
}

subroutine (AttenuationType) float Linear(float Distance)
{
	return 1 / (Distance + None(Distance));
}

subroutine (AttenuationType) float Quadratic(float Distance)
{
	return 1 / (pow(Distance, 2) + Linear(Distance));
}

subroutine (AttenuationType) float Cubic(float Distance)
{
	return 1 / (pow(Distance, 3) + Quadratic(Distance));
}

subroutine (DiffuseModel) vec3 TextureDiffuse()
{
	return texture(Material.TexSampler, TexCoord).rgb;
}

subroutine (DiffuseModel) vec3 NonTextureDiffuse()
{
	return Material.Kd;
}

vec3 OmniLightFragShading(vec3 vertex_position, vec3 vertex_normal, vec3 diffuse_factor)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0, 0, 0);

	vec3 EyeDirection = normalize(EyePosition - vertex_position);

	for(int LightIndex = 0; LightIndex < OmniLightNum; ++LightIndex)
	{
		float LightDistance = length(OmniLights[LightIndex].Position - vertex_position);

		vec3 LightDirection = normalize(OmniLights[LightIndex].Position - vertex_position);
		vec3 HalfVector = normalize(LightDirection + EyeDirection);

		float CosTheta = clamp(dot(vertex_normal, LightDirection), 0, 1);
		float CosAlpha = clamp(dot(HalfVector, vertex_normal), 0, 1);

		MaterialDiffuseColor = diffuse_factor * OmniLights[LightIndex].Color * OmniLights[LightIndex].Intensity * CosTheta * OmniLightAttenuations[LightIndex](LightDistance);
		MaterialSpecularColor = Material.Ks * OmniLights[LightIndex].Color * OmniLights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) * OmniLightAttenuations[LightIndex](LightDistance);

		FinalColor += LightDistance <= OmniLights[LightIndex].AreaRadius ? MaterialAmbientColor * MaterialDiffuseColor + MaterialDiffuseColor + MaterialSpecularColor : vec3(0,0,0);
	}

	return FinalColor;
}

vec3 DirectionalLightFragShading(vec3 vertex_position, vec3 vertex_normal, vec3 diffuse_factor)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0,0,0);

	vec3 EyeDirection = normalize(EyePosition - vertex_position);

	for(int LightIndex = 0; LightIndex < DirectionalLightNum; ++LightIndex)
	{
		float LightDistance = length(DirectionalLights[LightIndex].Position - vertex_position);
		vec3 LightDirection = normalize(DirectionalLights[LightIndex].Position - DirectionalLights[LightIndex].TargetPos);
		vec3 HalfVector = normalize(LightDirection + EyeDirection);

		float CosTheta = clamp(dot(vertex_normal, LightDirection), 0, 1);
		float CosAlpha = clamp(dot(HalfVector, vertex_normal), 0, 1);

		MaterialDiffuseColor = diffuse_factor * DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * CosTheta * DirectionalLightAttenuations[LightIndex](LightDistance);
		MaterialSpecularColor = Material.Ks * DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) * DirectionalLightAttenuations[LightIndex](LightDistance);

		FinalColor += LightDistance <= DirectionalLights[LightIndex].AreaRadius ? MaterialAmbientColor * MaterialDiffuseColor + MaterialDiffuseColor + MaterialSpecularColor : vec3(0,0,0);
	}

	return FinalColor;
}

vec3 SpotLightFragShading(vec3 vertex_position, vec3 vertex_normal, vec3 diffuse_factor)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialDiffuseColor, MaterialSpecularColor;
	vec3 FinalColor = vec3(0,0,0);

	vec3 EyeDirection = normalize(EyePosition - vertex_position);

	for(int LightIndex = 0; LightIndex < SpotLightNum; ++LightIndex)
	{
		float LightDistance = length(SpotLights[LightIndex].Position - vertex_position);
		vec3 LightDirection = normalize(SpotLights[LightIndex].Position - SpotLights[LightIndex].TargetPos);
		vec3 HalfVector = normalize(LightDirection + EyeDirection);

		vec3 LightToVertexDir = normalize(vertex_position - SpotLights[LightIndex].Position);
		float Visibility = 1.0f;
		if(clamp(dot(LightToVertexDir, -LightDirection), 0, 1) >= cos(SpotLights[LightIndex].CutOffAngle / 2))
		{
 			Visibility = 1.0f;
		}
		else
		{
			Visibility = 0.0f;
		}

		float CosTheta = clamp(dot(vertex_normal, LightDirection), 0, 1);
		float CosAlpha = clamp(dot(HalfVector, vertex_normal), 0, 1);

		MaterialDiffuseColor = Visibility * 0.01 * diffuse_factor * SpotLights[LightIndex].Color * SpotLights[LightIndex].Intensity * CosTheta * SpotLightAttenuations[LightIndex](LightDistance);
		MaterialSpecularColor = Visibility * 0.01 * Material.Ks * SpotLights[LightIndex].Color * SpotLights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) * SpotLightAttenuations[LightIndex](LightDistance);

		FinalColor += LightDistance <= SpotLights[LightIndex].AreaRadius ? MaterialAmbientColor * MaterialDiffuseColor + MaterialDiffuseColor + MaterialSpecularColor : vec3(0,0,0);
	}

	return FinalColor;
}

subroutine (RenderPassType) void GeometryPassFS()
{
	gPosition = Position;
	gNormal = Normal;
	gDiffuseFactor = DiffuseModelSelection();
}

subroutine (RenderPassType) void LightingPassFS()
{
	vec3 pos = vec3(texture(VertexPositionTex, TexCoord));
	vec3 n = normalize(vec3(texture(VertexNormalTex, TexCoord)));
	vec3 diffuse_factor = vec3(texture(VertexDiffuseTex, TexCoord));

	FragColor = vec4(OmniLightFragShading(pos, n, diffuse_factor), 1) 
			+ vec4(DirectionalLightFragShading(pos, n, diffuse_factor), 1)
			+ vec4(SpotLightFragShading(pos, n, diffuse_factor), 1);
}

void main()
{
	RenderPassSelectionFS();
}