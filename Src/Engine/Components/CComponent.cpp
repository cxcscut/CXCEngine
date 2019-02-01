#include "Components/CComponent.h"
#include "Actor/CActor.h"

namespace cxc
{
	CComponent::CComponent()
	{
		pOwnerObject.reset();
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

	void CComponent::SetOwnerObject(std::shared_ptr<CActor> InObject)
	{
		pOwnerObject = InObject;
	}
}