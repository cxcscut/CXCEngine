#include "Lighting.h"

namespace cxc {

	BaseLighting::BaseLighting(const glm::vec3 &pos, const glm::vec3 &dir,LightType type,InteractionType interactive)
		:LightPos(pos),LightDirection(dir),Type(type),Interactive(interactive)
	{

	}

	BaseLighting::~BaseLighting()
	{

	}

	void BaseLighting::SetLightPos(const glm::vec3 &pos) noexcept
	{
		LightPos = pos;
	}

	void BaseLighting::SetLightType(LightType type) noexcept
	{
		Type = type;
	}

	void BaseLighting::SetDirection(const glm::vec3 &dir) noexcept
	{
		LightDirection = dir;
	}

	glm::vec3 BaseLighting::GetDirection() const noexcept
	{
		return LightDirection;
	}

	LightType BaseLighting::GetLightType() const noexcept
	{
		return Type;
	}

	glm::vec3 BaseLighting::GetLightPos() const noexcept
	{
		return LightPos;
	}
}