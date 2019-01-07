#ifndef CXC_CCLUSTER_H
#define CXC_CCLUSTER_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	// Cluster mode
	using eClusterMode = eBlendMode;

	// Interpolation type
	enum class eInterpolationType : uint16_t
	{
		Lerp = 0, /* Linear interpolation */
		sLerp = 1  /* Spherical linear interpolation */,
		sQuard = 2  /* Spherical cubic spline interpolation */
	};

	class LinkBone;
	class Skin;

	/* Cluster which is a part of a Skin containsss a collection of vertices, the cluster contains the weights for each control point */
	class CXC_ENGINECORE_API Cluster
	{
	public:

		Cluster();
		~Cluster();

	public:

		glm::mat4 ComputeClusterDeformationMatrix();

	public:

		eClusterMode GetClusterMode() const { return ClusterMode; }
		std::shared_ptr<Skin> GetSkin();
		std::shared_ptr<LinkBone> GetLinkBone();
		glm::mat4 GetClusterModelMatrix() { return ClusterModelMatrix; }

	private:

		// ClusterMode
		eClusterMode ClusterMode;

		// AssociateModelMatrix, used when cluster model is Additive
		glm::mat4 AssociateModelMatrix;

		// Model matrix of the cluster
		glm::mat4 ClusterModelMatrix;

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
		std::weak_ptr<LinkBone> LinkBone;

		// Weak pointer back to the Skin that the cluster belongs to
		std::weak_ptr<Skin> OwnerSkin;
	};
}

#endif // CXC_CCLUSTER_H