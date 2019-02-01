#include "Components/CCameraComponent.h"
#include "Scene/Camera.h"

namespace cxc
{
	CCameraComponent::CCameraComponent():
		pCamera(nullptr)
	{

	}

	CCameraComponent::~CCameraComponent()
	{
		pCamera = nullptr;
	}
}