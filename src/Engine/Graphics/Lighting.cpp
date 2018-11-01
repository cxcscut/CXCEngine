#include "Lighting.h"

namespace cxc {

	BaseLighting::BaseLighting()
	{

	}

	BaseLighting::BaseLighting(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type, eInteractionType interactive)
		:LightPos(pos),LightDirection(dir), LightType(type),Interactive(interactive)
	{

	}

	BaseLighting::~BaseLighting()
	{

	}

	void BaseLighting::SetLightPos(const glm::vec3 &pos) noexcept
	{
		LightPos = pos;
	}

	void BaseLighting::SetLightType(eLightType type) noexcept
	{
		LightType = type;
	}

	void BaseLighting::SetDirection(const glm::vec3 &dir) noexcept
	{
		LightDirection = dir;
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