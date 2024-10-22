#ifndef CXC_SCENECOMPONENT_H
#define CXC_SCENECOMPONENT_H

#include "CComponent.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	/* CSceneComponent is the component that enables the CActor able to be placed in the world */
	class CXC_ENGINECORE_API CSceneComponent : public CComponent
	{
	public :
		CSceneComponent();
		virtual ~CSceneComponent();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		glm::mat4 EvaluateGlobalTransform();
		glm::mat4 EvaluateLocalTransform() const { return LocalTransformMatrix; }

		virtual void SetLocalTransform(const glm::mat4& Transform) { LocalTransformMatrix = Transform; }

	protected:

		// Relative transform matrix of the component
		glm::mat4 LocalTransformMatrix;
	};
}

#endif // CXC_SCENECOMPONENT_H