#ifndef CXC_CCLUSTER_H
#define CXC_CCLUSTER_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	// CCluster mode
	using eClusterMode = eBlendMode;

	class Mesh;
	class CLinkBone;

	/* CCluster which is a part of a CSkin containsss a collection of vertices, the cluster contains the weights for each control point */
	class CXC_ENGINECORE_API CCluster
	{
	public:
		friend class FBXSDKUtil;

		CCluster();
		~CCluster();

	public:

		glm::mat4 ComputeClusterDeformation(const glm::mat4& CurrentModelMatrix);

	public:

		void SetSkinnedMesh(std::shared_ptr<Mesh> pMesh) { pSkinedMesh = pMesh; }
		eClusterMode GetClusterMode() const { return ClusterMode; }
		std::shared_ptr<CLinkBone> GetLinkBone();
		glm::mat4 GetClusterInitialModelMatrix() { return ClusterInitialModelMatrix; }

	private:

		// AssociateModelMatrix, used when cluster model is Additive
		glm::mat4 AssociateModelMatrix;

		// Model matrix of the cluster
		glm::mat4 ClusterInitialModelMatrix;

	private:

		// ClusterMode
		eClusterMode ClusterMode;

		// Mesh
		std::shared_ptr<Mesh> pSkinedMesh;

		// Weights of the control points
		std::vector<float> ControlPointsWeight;

		// Indices of the control points
		std::vector<uint32_t> ControlPointsIndices;

		// Weak pointer back to the owner bone
		std::weak_ptr<CLinkBone> pOwnerBone;
	};
}

#endif // CXC_CCLUSTER_H