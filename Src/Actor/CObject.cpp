#include "Actor/CObject.h"
#include "Components/CComponent.h"

namespace cxc
{
	CObject::CObject() :
		bIsEnabled(true)
	{

	}

	CObject::CObject(const std::string& NewName)
	{
		Name = NewName; 
		bIsEnabled = true;
	}

	CObject::~CObject()
	{

	}

	std::shared_ptr<CComponent> CObject::GetComponent(size_t Index)
	{
		if (Index >= AttachedComponents.size())
			return false;
		else
			return AttachedComponents[Index];
	}

	void CObject::AddAttachedComponent(std::shared_ptr<CComponent> Component)
	{
		if(Component)
			AttachedComponents.push_back(Component);
	}

	void CObject::Tick(float DeltaSeconds)
	{
		// Tick all the components
		for (auto Component : AttachedComponents)
		{
			if (Component)
				Component->Tick(DeltaSeconds);
		}
	}
}