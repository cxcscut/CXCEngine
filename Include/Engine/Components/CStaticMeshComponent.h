#ifndef CXC_STATICMESH_COMPONENT_H
#define CXC_STATICMESH_COMPONENT_H

#include "CComponent.h"

namespace cxc
{
	class CComponent;
	class Mesh;

	class CXC_ENGINECORE_API CStaticMeshComponent : public CComponent
	{
	public:
		CStaticMeshComponent();
		CStaticMeshComponent(std::shared_ptr<Mesh> pMesh);
		virtual ~CStaticMeshComponent();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		std::shared_ptr<Mesh> GetStaticMesh() { return pStaticMesh; }
		void SetStaticMesh(std::shared_ptr<Mesh> pMesh) { pStaticMesh = pMesh; }

	private:

		// Static mesh of the component
		std::shared_ptr<Mesh> pStaticMesh;
	};
}

#endif // CXC_STATICMESH_COMPONENT_H