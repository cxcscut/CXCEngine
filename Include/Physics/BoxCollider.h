#ifndef CXC_BOXCOLLDER_H
#define CXC_BOXCOLLDER_H

#include "Physics/Collider3D.h"
#include "General/DefineTypes.h"

namespace cxc
{
	/* BoxCollider is the Collider3D that uses Box to handle collision */
	class CXC_ENGINECORE_API BoxCollider : public Collider3D
	{
	public:
		BoxCollider();
		virtual ~BoxCollider();

	public:

		void ModifyBoxColliderExtents(const glm::vec3& NewExtents);
		void CreateBoxColliderGeom(dSpaceID SpaceID, const glm::vec3& Extents);
		glm::vec3 GetBoxColliderExtents() const;
	};
}

#endif // CXC_BOXCOLLDER_H