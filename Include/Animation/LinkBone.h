#ifndef CXC_CLINKBONE_H
#define CXC_CLINKBONE_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CSkin;
	class CSkeleton;

	/* CLinkBone is the bone of a skeleton, which contain a collection of CSkins attached to it */
	class CXC_ENGINECORE_API CLinkBone : public std::enable_shared_from_this<CLinkBone>
	{
	public:
		CLinkBone(const std::string& BoneName, const glm::vec3& StartPos, const glm::vec3& EndPos);
		~CLinkBone();

	public:

		void AddChildBone(std::shared_ptr<CLinkBone> pChildBone);
		void SetParentBone(std::shared_ptr<CLinkBone> pParentBone);
		void SetOwnerSkeleton(std::shared_ptr<CSkeleton> OwnerSkeleton) { pOwnerSkeleton = OwnerSkeleton; }
		std::shared_ptr<CSkeleton> GetOwnerSkeleton();
		std::shared_ptr<CLinkBone> GetParentBone();

	public:

		std::string GetName() const { return Name; }

	private:

		// Name of the bone
		std::string Name;

		// Skins being attached to the bone
		std::vector<std::shared_ptr<CSkin>> Skins;

		// Start position of the bone
		glm::vec3 StartPosition;

		// End position of the bone
		glm::vec3 EndPosition;

		// Owner skeleton
		std::weak_ptr<CSkeleton> pOwnerSkeleton;

		// Parent bone
		std::weak_ptr<CLinkBone> ParentBone;

		// Child bones
		std::vector<std::shared_ptr<CLinkBone>> ChildBones;
	};
}

#endif // CXC_CLINKBONE_H