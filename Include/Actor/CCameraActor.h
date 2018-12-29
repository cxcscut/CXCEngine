#ifndef CXC_CCAMERAACTOR_H
#define CXC_CCAMERAACTOR_H

#include "Actor/CActor.h"

namespace cxc
{
	class Camera;

	/* CCameraActor is the actor that has CCameraComponent which represents the camera, it can be placed in the level */
	class CXC_ENGINECORE_API CCameraActor final : public CActor
	{
	public :
		CCameraActor();
		virtual ~CCameraActor();

	public:
		
		std::shared_ptr<Camera> GetCamera();
		void SetCamera(std::shared_ptr<Camera> Camera);
	};
}

#endif // CXC_CCAMERAACTOR_H