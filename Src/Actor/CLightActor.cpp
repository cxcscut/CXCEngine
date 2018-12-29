#include "Actor/CLightActor.h"
#include "Components/CLightComponent.h"
#include "Scene/Lighting.h"

namespace cxc
{
	CLightActor::CLightActor()
	{
		auto LightComponent = NewObject<CLightComponent>();
		SetRootComponent(LightComponent);
	}

	CLightActor::~CLightActor()
	{

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
		}
	}
}