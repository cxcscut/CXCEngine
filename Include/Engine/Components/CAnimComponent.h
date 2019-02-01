#ifndef CXC_ANIMCOMPONENT_H
#define CXC_ANIMCOMPONENT_H

#include "Components/CComponent.h"
#include <memory>

namespace cxc
{
	class AnimContext;

	class CXC_ENGINECORE_API CAnimComponent : public CComponent
	{
	public:
		CAnimComponent();
		virtual ~CAnimComponent();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		void CreateAnimContext();
		std::shared_ptr<AnimContext> GetAnimContext() { return pAnimContext; }

	protected:

		std::shared_ptr<AnimContext> pAnimContext;
	};
}

#endif // CXC_ANIMCOMPONENT_H