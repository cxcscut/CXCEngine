#include "Actor/CLightActor.h"
#include "Components/CLightComponent.h"
#include "Scene/Lighting.h"

namespace cxc
{
	CLightActor::CLightActor()
		: CActor()
	{
		auto LightComponent = NewObject<CLightComponent>();
		RootComponent = LightComponent;
		AttachComponent(LightComponent);
	}

	CLightActor::CLightActor(const std::string& LightName):
		CActor(LightName)
	{
		auto LightComponent = NewObject<CLightComponent>();
		RootComponent = LightComponent;
		AttachComponent(LightComponent);
	}

	CLightActor::~CLightActor()
	{

	}

	void CLightActor::Tick(float DeltaSeconds)
	{
		auto pLight = GetLight();
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

	std::shared_ptr<LightSource> CLightActor::GetLight()
	{
		auto LightComponent = std::dynamic_pointer_cast<CLightComponent>(RootComponent);
		if (LightComponent)
		{
			return LightComponent->GetLight();
		}
		else
			return nullptr;
	}

	void CLightActor::SetLight(std::shared_ptr<LightSource> Light)
	{
		auto LightComponent = std::dynamic_pointer_cast<CLightComponent>(RootComponent);
		if (LightComponent)
		{
			LightComponent->SetLight(Light);
			LightComponent->SetOwnerObject(shared_from_this());
		}
	}
}