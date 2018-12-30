#include "Actor/CCameraActor.h"
#include "Components/CCameraComponent.h"

namespace cxc
{
	CCameraActor::CCameraActor():
		CActor()
	{
		auto pCameraComponent = NewObject<CCameraComponent>();
		RootComponent = pCameraComponent;
		AttachComponent(pCameraComponent);
	}

	CCameraActor::CCameraActor(const std::string& Name):
		CActor(Name)
	{
		
	}

	void CCameraActor::Tick(float DeltaSeconds)
	{
		
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
			CameraComponent->SetOwnerObject(shared_from_this());
		}
	}
}