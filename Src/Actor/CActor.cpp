#include "Actor/CActor.h"
#include "Components/CComponent.h"

namespace cxc
{
	CActor::CActor() :
		bIsEnabled(true)
	{

	}

	CActor::CActor(const std::string& NewName)
	{
		Name = NewName; 
		bIsEnabled = true;
	}

	CActor::~CActor()
	{
		AttachedComponents.clear();
		RootComponent = nullptr;
	}

	std::shared_ptr<CComponent> CActor::GetComponent(size_t Index)
	{
		if (Index >= AttachedComponents.size())
			return false;
		else
			return AttachedComponents[Index];
	}

	void CActor::Tick(float DeltaSeconds)
	{
		// Tick all the components
		for (auto Component : AttachedComponents)
		{
			if (Component)
				Component->Tick(DeltaSeconds);
		}
	}

	void CActor::SetRootComponent(std::shared_ptr<CComponent> Root)
	{
		if (Root != nullptr && AttachComponent(Root))
		{
			RootComponent = Root;
			RootComponent->SetOwnerObject(shared_from_this());
		}
	}

	bool CActor::AttachComponent(std::shared_ptr<CComponent> InComponent)
	{
		if (InComponent != nullptr)
		{
			AttachedComponents.push_back(InComponent);
			return true;
		}

		return false;
	}
}