#include "Lighting.h"

namespace cxc {

	BaseLighting::BaseLighting()
	{

	}

	BaseLighting::BaseLighting(const std::string& Name, const glm::vec3 &pos, const glm::vec3 &dir, float Intensity, eLightType type)
		:LightPos(pos),LightDirection(dir), LightType(type), LightName(Name), LightIntensity(Intensity)
	{

	}

	BaseLighting::~BaseLighting()
	{

	}

	void BaseLighting::SetLightPos(float x, float y, float z) noexcept
	{
		LightPos = glm::vec3(x, y, z);
	}

	void BaseLighting::SetLightType(eLightType type) noexcept
	{
		LightType = type;
	}

	void BaseLighting::SetDirection(float x, float y, float z) noexcept
	{
		LightDirection = glm::vec3(x, y, z);
	}

	glm::vec3 BaseLighting::GetDirection() const noexcept
	{
		return LightDirection;
	}

	eLightType BaseLighting::GetLightType() const noexcept
	{
		return LightType;
	}

	glm::vec3 BaseLighting::GetLightPos() const noexcept
	{
		return LightPos;
	}
}