#ifndef CXC_RENDERERCONTEXT_H
#define CXC_RENDERERCONTEXT_H

#include <memory>
#include <vector>
#include "Core/EngineCore.h"

namespace cxc
{
	class LightSource;
	class SubMeshRenderer;
	class SubMesh;

	/* RendererContext is an instance that stores the runtime information of a renderer */ 
	class CXC_ENGINECORE_API RendererContext
	{
	public:

		RendererContext(std::shared_ptr<SubMeshRenderer> pRenderer);
		~RendererContext();

	public:

		void SetShadingLights(const std::vector<std::shared_ptr<LightSource>>& Lights) { ShadingLights = Lights; }
		void AddBindedSubMesh(std::shared_ptr<SubMesh> pSubMesh);
		void RemoveBindedSubMesh(std::shared_ptr<SubMesh> pSubMesh);
		
		std::shared_ptr<SubMeshRenderer> GetRenderer();
		const std::vector<std::shared_ptr<SubMesh>>& GetBindedSubMeshes() const { return SubMeshes; }
		const std::vector<std::shared_ptr<LightSource>>& GetLightsArray() const { return ShadingLights; }

	private:

		// Lights need to be shaded
		std::vector<std::shared_ptr<LightSource>> ShadingLights;

		// SubMeshes that binded to the renderer
		std::vector<std::shared_ptr<SubMesh>> SubMeshes;

		// Weak pointer back to the SubMeshRenderer
		std::weak_ptr<SubMeshRenderer> pSubMeshRenderer;

	};
}

#endif // CXC_RENDERERCONTEXT_H