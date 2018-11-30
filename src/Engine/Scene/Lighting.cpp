#include "Lighting.h"

namespace cxc {

	LightSource::LightSource()
	{

	}

	LightSource::LightSource(const std::string& Name, const glm::vec3 &pos, const glm::vec3 &dir, float Intensity, eLightType type)
		:LightPos(pos),LightDirection(dir), LightType(type), LightName(Name), LightIntensity(Intensity)
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

	void LightSource::SetDirection(float x, float y, float z) noexcept
	{
		LightDirection = glm::vec3(x, y, z);
	}

	glm::vec3 LightSource::GetDirection() const noexcept
	{
		return LightDirection;
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