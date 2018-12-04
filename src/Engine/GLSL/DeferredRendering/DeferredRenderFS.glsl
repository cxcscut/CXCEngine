#version 430 core
#define DEFERRED_MAX_LIGHTS_NUM 1024

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
uniform LightSource Lights[DEFERRED_MAX_LIGHTS_NUM];
uniform int LightNum;

uniform vec3 EyePosition;

subroutine void RenderPassType();
subroutine vec3 DiffuseModel();
subroutine uniform RenderPassType RenderPassSelection;
subroutine uniform DiffuseModel DiffuseModelSelection;

uniform sampler2D VertexPositionTex;
uniform sampler2D VertexNormalTex;
uniform sampler2D VertexDiffuseTex;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec3 gDiffuseFactor;
layout (location = 3) out vec3 gNormal;

subroutine (DiffuseModel) vec3 TextureDiffuse()
{
	return texture(Material.TexSampler, TexCoord).rgb;
}

subroutine (DiffuseModel) vec3 NonTextureDiffuse()
{
	return Material.Kd;
}

subroutine (RenderPassType) void GeometryPass()
{
	gPosition = Position;
	gNormal = Normal;
	gDiffuseFactor = DiffuseModelSelection();
}

vec3 FragShading(vec3 vertex_position, vec3 vertex_normal, vec3 diffuse_factor)
{
	vec3 MaterialAmbientColor = Material.Ka * vec3(0.2, 0.2, 0.2);
	vec3 MaterialAlbedoColor;

	for(int LightIndex = 0; LightIndex < LightNum; ++LightIndex)
	{
		float LightDistance = length(Lights[LightIndex].Position - vertex_position);
		vec3 LightDirection = normalize(vertex_position - Lights[LightIndex].Position);
		vec3 EyeDirection = normalize(EyePosition - vertex_position);
		vec3 HalfVector = normalize(LightDirection + EyeDirection);

		float CosTheta = clamp(dot(vertex_normal, LightDirection), 0, 1);
		float CosAlpha = clamp(dot(HalfVector, vertex_normal), 0, 1);

		vec3 MaterialDiffuseColor = diffuse_factor * Lights[LightIndex].Color * Lights[LightIndex].Intensity * CosTheta / LightDistance;
		vec3 MaterialSpecularColor = Material.Ks * Lights[LightIndex].Color * Lights[LightIndex].Intensity * pow(CosAlpha, Material.Shiniess) / LightDistance;
	}

	return MaterialAmbientColor + MaterialAlbedoColor;
}

subroutine (RenderPassType) void LightingPass()
{
	vec3 pos = vec3(texture(VertexPositionTex, TexCoord));
	vec3 n = vec3(texture(VertexNormalTex, TexCoord));
	vec3 diffuse_factor = vec3(texture(VertexDiffuseTex, TexCoord));

	FragColor = vec4(FragShading(pos, n, diffuse_factor), 1);
}

void main()
{
	RenderPassSelection();
}