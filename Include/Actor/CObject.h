#ifndef CXC_COBJECT_H
#define CXC_COBJECT_H

#include "General/DefineTypes.h"

namespace cxc
{
	class CComponent;
	class LightSource;

	/* A CObject is the base class for all the placeable object in the world */
	class CObject : public std::enable_shared_from_this<CObject>
	{
	public:

		CObject();
		CObject(const std::string& Name);
		virtual ~CObject();

	public:

		glm::mat4 GetTransformMatrix() const { return TransformMatrix; }
		std::shared_ptr<CComponent> GetRootComopnent() { return RootComponent; }
		std::string GetName() const { return Name; }
		std::string CompareTag() const { return Tag; }
		bool IsEnable() const { return bIsEnabled; }

		void AddAttachedComponent(std::shared_ptr<CComponent> Component);
		void SetRootComponent(std::shared_ptr<CComponent> Root);
		void SetEnable(bool Status) { bIsEnabled = Status; }
		void SetTag(const std::string& NewTag) { Tag = NewTag; }
		void SetName(const std::string& NewName) { Name = NewName; }

	public:

		virtual void Tick(float DeltaSeconds);

	public:

		template<class ComponentClass>
		bool AttachComponent(std::shared_ptr<ComponentClass> InComponent)
		{
			bool bComponentClassTypeExist = false;
			for (auto Component : AttachedComponents)
			{
				bComponentClassTypeExist |= std::dynamic_pointer_cast<ComponentClass>(Component) != nullptr;
			}

			if (!bComponentClassTypeExist)
			{
				auto pComponent = std::dynamic_pointer_cast<CComponent>(InComponent);
				if (pComponent != nullptr)
				{
					AttachedComponents.push_back(InComponent);
					return true;
				}
			}

			return false;
		}

		template<class ComponentClass>
		std::shared_ptr<ComponentClass> GetComponent()
		{
			for (auto Component : AttachedComponents)
			{
				auto RetComponent = std::dynamic_pointer_cast<ComponentClass>(Component);
				if (RetComponent != nullptr)
					return RetComponent;
			}

			return nullptr;
		}

	protected:
		
		// Tranform matrix for the object
		glm::mat4 TransformMatrix;

		// Whether the object is enabled
		bool bIsEnabled;

		// Name of the CObject
		std::string Name;

		// Tag of the object
		std::string Tag;

		// Root component
		std::shared_ptr<CComponent> RootComponent;

		// Components attached to the object
		std::vector<std::shared_ptr<CComponent>> AttachedComponents;

		// Parent node
		std::weak_ptr<CObject> ParentNode;

		// Child nodes
		std::vector<std::shared_ptr<CObject>> ChildNodes;

	};
}

#endif // CXC_COBJECT_H