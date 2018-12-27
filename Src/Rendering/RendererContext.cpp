#include "Rendering/RendererContext.h"
#include "Rendering/SubMeshRenderer.h"
#include "Geometry/SubMesh.h"

namespace cxc
{
	RendererContext::RendererContext(std::shared_ptr<SubMeshRenderer> pRenderer)
	{
		pSubMeshRenderer.reset();
		pSubMeshRenderer = pRenderer;
	}

	RendererContext::~RendererContext()
	{
		SubMeshes.clear();
		pSubMeshRenderer.reset();
	}

	void RendererContext::AddBindedSubMesh(std::shared_ptr<SubMesh> pSubMesh)
	{
		if (pSubMesh)
		{
			bool bIsSubMeshExist = false;
			for (auto SubMesh : SubMeshes)
			{
				bIsSubMeshExist |= SubMesh == pSubMesh;
			}

			if (!bIsSubMeshExist)
				SubMeshes.push_back(pSubMesh);
		}
	}

	void RendererContext::RemoveBindedSubMesh(std::shared_ptr<SubMesh> pSubMesh)
	{
		for (auto iter = SubMeshes.begin(); iter != SubMeshes.end(); ++iter)
		{
			if (*iter == pSubMesh)
			{
				SubMeshes.erase(iter);
			}
		}
	}

	std::shared_ptr<SubMeshRenderer> RendererContext::GetRenderer()
	{
		if (!pSubMeshRenderer.expired())
			return pSubMeshRenderer.lock();
		else
			return nullptr;
	}
}