#include "Actor/CCameraActor.h"
#include "Components/CCameraComponent.h"

namespace cxc
{
	CCameraActor::CCameraActor()
	{
		auto pCameraComponent = NewObject<CCameraComponent>();
		SetRootComponent(pCameraComponent);
	}

	CCameraActor::~CCameraActor()
	{

	}

	std::shared_ptr<Camera> CCameraActor::GetCamera()
	{
		auto CameraComponent = std::dynamic_pointer_cast<CCameraComponent>(RootComponent);
		if (CameraComponent)
		{
			return CameraComponent->GetCamera();
		}
		else
			return nullptr;
	}

	void CCameraActor::SetCamera(std::shared_ptr<Camera> Camera)
	{
		auto CameraComponent = std::dynamic_pointer_cast<CCameraComponent>(RootComponent);
		if (CameraComponent)
		{
			CameraComponent->SetCamera(Camera);
		}
	}
}