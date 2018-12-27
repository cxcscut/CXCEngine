#ifndef CXC_SCENECOMPONENT_H
#define CXC_SCENECOMPONENT_H

#include "CComponent.h"
#include "General/DefineTypes.h"

namespace cxc
{
	/* CSceneComponent is the component that enables the CObject able to be placed in the world */
	class CSceneComponent : public CComponent
	{
	public :
		CSceneComponent();
		virtual ~CSceneComponent();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		glm::mat4 EvaluateGlobalTransform();
		glm::mat4 EvaluateLocalTransform() const { return LocalTransformMatrix; }

		void SetLocalTransform(const glm::mat4& Transform) { LocalTransformMatrix = Transform; }

	protected:

		glm::mat4 LocalTransformMatrix;
	};
}

#endif // CXC_SCENECOMPONENT_H