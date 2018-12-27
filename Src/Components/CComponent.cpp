#include "Components/CComponent.h"
#include "Actor/CObject.h"

namespace cxc
{
	CComponent::CComponent()
	{

	}

	CComponent::~CComponent()
	{

	}

	void CComponent::Tick(float DeltaSeconds)
	{

	}

	void CComponent::SetParentNode(std::shared_ptr<CComponent> InComponent)
	{
		ParentComponent = InComponent;
	}

	void CComponent::SetOwnerObject(std::shared_ptr<CObject> InObject)
	{
		pOwnerObject = InObject;
	}
}