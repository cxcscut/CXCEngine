#include "Scene/Lighting.h"

namespace cxc {

	LightSource::LightSource():
		AreaRadius(10000.0f)
	{

	}

	LightSource::LightSource(const std::string& Name, const glm::vec3 &pos, const glm::vec3 &TargetPos, float Intensity, eLightType type)
		:LightPos(pos), TargetPos(TargetPos), LightType(type), LightName(Name), LightIntensity(Intensity),
		AtteunationType(eLightAtteunationType::Linear)
	{

	}

	LightSource::~LightSource()
	{

	}

	void LightSource::SetLightPos(float x, float y, float z) noexcept
	{
		LightPos = glm::vec3(x, y, z);
	}

	void LightSource::SetLightType(eLightType type) noexcept
	{
		LightType = type;
	}

	void LightSource::SetTargetPos(float x, float y, float z) noexcept
	{
		TargetPos = glm::vec3(x, y, z);
	}

	glm::vec3 LightSource::GetTargetPos() const noexcept
	{
		return TargetPos;
	}

	eLightType LightSource::GetLightType() const noexcept
	{
		return LightType;
	}

	glm::vec3 LightSource::GetLightPos() const noexcept
	{
		return LightPos;
	}
}