#ifndef CXC_MOVEMENTCTRLSYSTEM_H
#define CXC_MOVEMENTCTRLSYSTEM_H

#include "CSystem.h"
#include <Memory>

namespace cxc
{
	class CComponent;

	/* CMovementCtrlSystem is the class that manipulates the movement of an object */
	class CXC_ENGINECORE_API CMovementCtrlSystem : public CSystem
	{
	public:
		CMovementCtrlSystem();
		~CMovementCtrlSystem();

	public:

		virtual void Translate(std::shared_ptr<CComponent> Component, float x, float y, float z);
		virtual void RotateWorldSpace(std::shared_ptr<CComponent> Component, float x, float y, float z);
		virtual void RotateLocalSpace(std::shared_ptr<CComponent> Component, float x, float y, float z);
		virtual void RotateWithArbitraryAxis(std::shared_ptr<CComponent> Component,
			float pos_x, float pos_y, float pos_z, float x, float y, float z);
	};
}

#endif // CXC_MOVEMENTCTRLSYSTEM_H