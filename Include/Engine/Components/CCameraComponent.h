#ifndef CXC_CAMERACOMPONENT_H
#define CXC_CAMERACOMPONENT_H

#include "Components/CSceneComponent.h"
#include <memory>

namespace cxc
{
	class Camera;

	/* CCameraComponent is the component that represents a camera, which can be attached to an actor and placed in the level */
	class CXC_ENGINECORE_API CCameraComponent : public CSceneComponent
	{
		
	public:
		CCameraComponent();
		virtual ~CCameraComponent();

	public:

		void SetCamera(std::shared_ptr<Camera> Camera) { pCamera = Camera; }
		std::shared_ptr<Camera> GetCamera() { return pCamera; }

	private:

		std::shared_ptr<Camera> pCamera;
	};
}

#endif //CXC_CAMERACOMPONENT_H