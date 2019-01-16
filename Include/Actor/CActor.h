#ifndef CXC_CACTOR_H
#define CXC_CACTOR_H

#include "Core/EngineTypes.h"
#include "Core/CObject.h"

namespace cxc
{
	class CComponent;
	class LightSource;
	class LightActor;

	/* A CActor is the base class for all the placeable object in the world */
	class CXC_ENGINECORE_API CActor : public CObject, public std::enable_shared_from_this<CActor>
	{
	public:

		CActor();
		CActor(const std::string& Name);
		virtual ~CActor();

	public:

		virtual void Initialize() override;

	public:

		glm::mat4 GetTransformMatrix() const { return TransformMatrix; }
		std::string GetName() const { return Name; }
		std::string CompareTag() const { return Tag; }
		bool IsEnable() const { return bIsEnabled; }

		void SetEnable(bool Status) { bIsEnabled = Status; }
		void SetTag(const std::string& NewTag) { Tag = NewTag; }
		void SetName(const std::string& NewName) { Name = NewName; }

	public:

		size_t GetComponentCount() const { return AttachedComponents.size(); }
		std::shared_ptr<CComponent> GetComponent(size_t Index);
		std::shared_ptr<CComponent> GetRootComopnent() { return RootComponent; }

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		void SetRootComponent(std::shared_ptr<CComponent> Root);
		bool AttachComponent(std::shared_ptr<CComponent> InComponent);

	protected:

		// Tranform matrix for the object
		glm::mat4 TransformMatrix;

		// Whether the object is enabled
		bool bIsEnabled;

		// Name of the CActor
		std::string Name;

		// Tag of the object
		std::string Tag;

		// Root component
		std::shared_ptr<CComponent> RootComponent;

		// Components attached to the object
		std::vector<std::shared_ptr<CComponent>> AttachedComponents;

		// Parent node
		std::weak_ptr<CActor> ParentNode;

		// Child nodes
		std::vector<std::shared_ptr<CActor>> ChildNodes;

	};
}

#endif // CXC_CACTOR_H