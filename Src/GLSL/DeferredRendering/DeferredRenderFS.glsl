#version 430 core
#define DEFERRED_MAX_LIGHTS_NUM 64

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec3 gDiffuseFactor;
layout (location = 3) out vec3 gNormal;

subroutine float AtteunationType(float Distance);
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
	float Intensity;
};

uniform MaterialProperties Material;
uniform LightSource OmniLights[DEFERRED_MAX_LIGHTS_NUM];
uniform LightSource DirectionalLights[DEFERRED_MAX_LIGHTS_NUM];
uniform int OmniLightNum;
uniform int DirectionalLightNum;
uniform vec3 EyePosition;

subroutine uniform AtteunationType OmniLightAttenuations[DEFERRED_MAX_LIGHTS_NUM];
subroutine uniform AtteunationType DirectionalLightAtteunations[DEFERRED_MAX_LIGHTS_NUM];
subroutine uniform RenderPassType RenderPassSelectionFS;
subroutine uniform DiffuseModel DiffuseModelSelection;

uniform sampler2D VertexPositionTex;
uniform sampler2D VertexNormalTex;
uniform sampler2D VertexDiffuseTex;

layout (index = 0) subroutine (AtteunationType) float None(float Distance)
{
	return 1.0f;
}

subroutine (AtteunationType) float Linear(float Distance)
{
	return 1 / Distance;
}

subroutine (AtteunationType) float Quadratic(float Distance)
{
	return 1 / pow(Distance, 2);
}

subroutine (AtteunationType) float Cubic(float Distance)
{
	return 1 / pow(Distance, 3);
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

		FinalColor += MaterialAmbientColor * MaterialDiffuseColor + 
					MaterialDiffuseColor + 
					MaterialSpecularColor;
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

		MaterialDiffuseColor = diffuse_factor * DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * CosTheta * DirectionalLightAtteunations[LightIndex](LightDistance);
		MaterialSpecularColor = Material.Ks * DirectionalLights[LightIndex].Color * DirectionalLights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) * DirectionalLightAtteunations[LightIndex](LightDistance);

		FinalColor += MaterialAmbientColor * MaterialDiffuseColor +
				MaterialDiffuseColor +
				MaterialSpecularColor;
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
	vec3 n = vec3(texture(VertexNormalTex, TexCoord));
	vec3 diffuse_factor = vec3(texture(VertexDiffuseTex, TexCoord));

	FragColor = vec4(OmniLightFragShading(pos, n, diffuse_factor), 1) 
			+ vec4(DirectionalLightFragShading(pos, n, diffuse_factor), 1);
}

void main()
{
	RenderPassSelectionFS();
}