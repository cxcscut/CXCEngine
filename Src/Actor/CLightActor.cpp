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
		CActor::Tick(DeltaSeconds);
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