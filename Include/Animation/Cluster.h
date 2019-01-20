#ifndef CXC_CCLUSTER_H
#define CXC_CCLUSTER_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	// CCluster mode
	using eClusterMode = eBlendMode;

	class CLinkBone;
	class CSkin;

	/* CCluster which is a part of a CSkin containsss a collection of vertices, the cluster contains the weights for each control point */
	class CXC_ENGINECORE_API CCluster
	{
	public:

		CCluster();
		~CCluster();

	public:

		glm::mat4 ComputeClusterDeformation(const glm::mat4& CurrentModelMatrix);

	public:

		eClusterMode GetClusterMode() const { return ClusterMode; }
		std::shared_ptr<CSkin> GetSkin();
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

		// Indices of the control points
		std::vector<uint32_t> Indices;

		// Control points
		std::vector<glm::vec3> ControlPoints;

		// Normals of the control points;
		std::vector<glm::vec3> Normals;

		// UVs of the control points
		std::vector<glm::vec2> UVs;

		// Weights of the control points
		std::vector<float> ControlPointsWeight;

		// Weak pointer to the CBone that the cluster is attached to
		std::weak_ptr<CLinkBone> CLinkBone;

		// Weak pointer back to the CSkin that the cluster belongs to
		std::weak_ptr<CSkin> OwnerSkin;
	};
}

#endif // CXC_CCLUSTER_H