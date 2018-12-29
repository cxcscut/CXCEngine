#ifndef CXC_CLIGHTACTOR_H
#define CXC_CLIGHTACTOR_H

#include "Actor/CActor.h"
#include <memory>

namespace cxc
{
	class LightSource;

	class CLightActor : public CActor
	{
	public:
		CLightActor();
		virtual ~CLightActor();

	public:

		std::shared_ptr<LightSource> GetLight();
		void SetLight(std::shared_ptr<LightSource> Light);
	};
}

#endif // CXC_CLIGHTACTOR_H