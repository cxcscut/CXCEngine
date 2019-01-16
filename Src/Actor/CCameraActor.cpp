#include "Actor/CCameraActor.h"
#include "Components/CCameraComponent.h"
#include "Scene/Camera.h"

namespace cxc
{
	CCameraActor::CCameraActor():
		CActor()
	{
		auto pCameraComponent = NewObject<CCameraComponent>();
		RootComponent = pCameraComponent;
		AttachComponent(pCameraComponent);
	}

	CCameraActor::CCameraActor(std::shared_ptr<Camera> pCamera):
		CActor(pCamera->CameraName)
	{
		auto pCameraComponent = NewObject<CCameraComponent>();
		pCameraComponent->SetCamera(pCamera);
		RootComponent = pCameraComponent;
		AttachComponent(pCameraComponent);
	}

	void CCameraActor::Initialize()
	{
		CActor::Initialize();

		RootComponent->SetOwnerObject(shared_from_this());
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
			Name = Camera->CameraName;
		}
	}
}