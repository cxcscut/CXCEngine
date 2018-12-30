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
		CLightActor(const std::string& Name);
		virtual ~CLightActor();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		std::shared_ptr<LightSource> GetLight();
		void SetLight(std::shared_ptr<LightSource> Light);
	};
}

#endif // CXC_CLIGHTACTOR_H