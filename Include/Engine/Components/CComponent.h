#ifndef CXC_CCOMPONENT_H
#define CXC_CCOMPONENT_H

#include "Core/EngineTypes.h"
#include "Core/CObject.h"

namespace cxc
{
	class CActor;

	/* CComponent is the base class for all the component */
	class CXC_ENGINECORE_API CComponent : public CObject, public std::enable_shared_from_this<CComponent>
	{
	public:
		CComponent();
		virtual ~CComponent();

	public:
		
		void SetParentNode(std::shared_ptr<CComponent> InComponent);
		void SetOwnerObject(std::shared_ptr<CActor> InObject);

		std::shared_ptr<CComponent> GetParentComponent() { return ParentComponent.lock(); }
		std::shared_ptr<CActor> GetOwnerObject() { return pOwnerObject.lock(); }

	public:

		template<class ComponentTypeBaseClass>
		bool AttachComponent(std::shared_ptr<ComponentTypeBaseClass> Component)
		{
			if (pOwnerObject.lock() && pOwnerObject.lock()->AttachComponent<ComponentTypeBaseClass>(Component))
			{
				Component->SetParentComponent(shared_from_this());
				Component->SetOwnerObject(pOwnerObject.lock());
				ChildComponents.push_back(Component);
			}
		}

	public:

		virtual void Tick(float DeltaSeconds) override;

	protected:

		// Weak pointer back to the CActor that the component being attached to
		std::weak_ptr<CActor> pOwnerObject;

		// Weak pointer back to the parent component
		std::weak_ptr<CComponent> ParentComponent;

		// Child component nodes
		std::vector<std::shared_ptr<CComponent>> ChildComponents;
	};
}

#endif // CXC_CCOMPONENT_H