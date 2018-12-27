#ifndef CXC_CCOMPONENT_H
#define CXC_CCOMPONENT_H

#include "General/DefineTypes.h"

namespace cxc
{
	class CObject;

	/* CComponent is the base class for all the component */
	class CComponent
	{
	public:
		CComponent();
		~CComponent();

	public:
		
		template<class ComponentTypeBaseClass>
		bool AttachComponent(std::shared_ptr<ComponentTypeBaseClass> Component)
		{
			if (pOwnerObject.lock() && pOwnerObject.lock()->AttachComponent<ComponentTypeBaseClass>(Component))
			{
				ChildComponents.push_back(Component);
			}
		}

	public:

		glm::mat4 EvaluateGlobalTransform();

	public:

		virtual void Tick(float DeltaSeconds);

	private:

		// The transform matrix relative to the parent component
		glm::mat4 RelativeTransformMatrix;

		// Weak pointer back to the CObject that the component being attached
		std::weak_ptr<CObject> pOwnerObject;

		// Weak pointer back to the parent component
		std::weak_ptr<CComponent> ParentComponent;

		// Child component nodes
		std::vector<std::shared_ptr<CComponent>> ChildComponents;
	};
}

#endif // CXC_CCOMPONENT_H