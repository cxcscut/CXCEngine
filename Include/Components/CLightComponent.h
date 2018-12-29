#ifndef CXC_CLIGHTCOMPONENT_H
#define CXC_CLIGHTCOMPONENT_H

#include "Components/CSceneComponent.h"
#include <memory>

namespace cxc
{
	class LightSource;

	class CXC_ENGINECORE_API CLightComponent : public CSceneComponent
	{
	public:
		CLightComponent();
		virtual ~CLightComponent();

	public:

		void SetLight(std::shared_ptr<LightSource> Light) { pLight = Light; }
		std::shared_ptr<LightSource> GetLight() { return pLight; }

	private:

		std::shared_ptr<LightSource> pLight;
	};
}

#endif // CXC_CLIGHTCOMPONENT_H