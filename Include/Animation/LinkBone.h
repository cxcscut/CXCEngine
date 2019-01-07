#ifndef CXC_CLINKBONE_H
#define CXC_CLINKBONE_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class Skin;

	/* LinkBone is the bone of a skeleton, which contain a collection of CSkins attached to it */
	class CXC_ENGINECORE_API LinkBone
	{
	public:
		LinkBone();
		~LinkBone();

	public:

		std::string GetName() const { return Name; }
		glm::mat4 GetBoneModelMatrix() const { return BoneModelMatrix; }

	private:

		// Name of the bone
		std::string Name;

		// Skins being attached to the bone
		std::vector<std::shared_ptr<Skin>> Skins;

		// Model matrix of the bone
		glm::mat4 BoneModelMatrix;
	};
}

#endif // CXC_CLINKBONE_H