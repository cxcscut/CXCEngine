#include "Components/CComponent.h"
#include "Actor/CObject.h"

namespace cxc
{
	CComponent::CComponent() :
		RelativeTransformMatrix(glm::mat4(1.0f))
	{

	}

	CComponent::~CComponent()
	{

	}

	void CComponent::Tick(float DeltaSeconds)
	{

	}
}