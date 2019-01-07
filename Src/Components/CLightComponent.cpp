#include "Components/CLightComponent.h"
#include "Scene/Lighting.h"

namespace cxc
{
	CLightComponent::CLightComponent()
	{

	}

	CLightComponent::~CLightComponent()
	{

	}

	void CLightComponent::Tick(float DeltaSeconds)
	{
		if (pLight)
		{
			// Compute the angles of the light
			auto LightPos = pLight->GetLightPos();
			auto ThetaXOY = atan2(LightPos.x, LightPos.y);
			auto DeltaTheta = glm::radians(1.0f);

			// Apply a delta to the angle
			ThetaXOY += DeltaTheta;

			// Compute the position 
			auto RadiusXOY = glm::length(glm::vec2(LightPos.x, LightPos.y));
			pLight->SetLightPos(RadiusXOY * sinf(ThetaXOY), RadiusXOY * cosf(ThetaXOY), LightPos.z);
		}
	}
}