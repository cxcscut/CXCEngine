#include "Components/CAnimComponent.h"
#include "Animation/AnimContext.h"

namespace cxc
{
	CAnimComponent::CAnimComponent()
	{
		
	}

	CAnimComponent::~CAnimComponent()
	{
		pAnimContext = nullptr;
	}

	void CAnimComponent::CreateAnimContext()
	{
		pAnimContext = NewObject<AnimContext>(std::dynamic_pointer_cast<CAnimComponent>(shared_from_this()));
	}
}