#include "Animation/AnimContext.h"
#include "Animation/AnimStack.h"
#include "Animation/AnimLayer.h"
#include "Scene/Object3D.h"

namespace cxc
{
	AnimContext::AnimContext(std::shared_ptr<Object3D> pObject):
		pCurrentActiveAnimStack(nullptr), CurrentTime(0.0f)
	{
		pOwnerObject.reset();
		pOwnerObject = pObject;
	}

	AnimContext::~AnimContext()
	{

	}

	void AnimContext::Tick(float DeltaSeconds)
	{
		if (pOwnerObject.expired())
			return;

		if (pCurrentActiveAnimStack && pOwnerObject.lock())
		{
			CurrentTime += DeltaSeconds;

			auto SrcVertices = pOwnerObject.lock()->m_VertexCoords;
			pCurrentActiveAnimStack->Evaluate(CurrentTime, PlayMode, SrcVertices, DeformedVertices);
		}
	}

	void AnimContext::SetOwnerObject(std::shared_ptr<Object3D> pObject)
	{
		pOwnerObject = pObject;
	}

	std::shared_ptr<Object3D> AnimContext::GetOwnerObject()
	{
		return pOwnerObject.lock();
	}
}