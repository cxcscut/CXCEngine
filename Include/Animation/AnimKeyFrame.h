#ifndef CXC_ANIMATIONKEYFRAME_H
#define CXC_ANIMATIONKEYFRAME_H

#include "General/DefineTypes.h"

namespace cxc
{
	// Linking mode of each bones of the skeleton, all the linking mode must be the same
	enum class eBoneLinkingMode : uint16_t
	{
		Additive = 0,
		Normalized = 1,
		TotalOne = 2
	};

	// AnimKeyFrame is the class that encapsulates a frame of the skeleton animation
	class AnimKeyFrame final
	{
	public:
		AnimKeyFrame(float Time, eBoneLinkingMode LinkMode,
			const std::vector<float>& Weights,
			const std::vector<glm::mat4>& DeformationMatrices,
			const std::vector<std::vector<uint32_t>>& InfluencedIndices);

		~AnimKeyFrame();

	public:

		float GetStartTime() const { return StartTime; }
		eBoneLinkingMode GetLinkingMode() const { return LinkingMode; }

	public:

		void Evaluate(const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);

	private:

		void DeformVertex(const glm::mat4 DeformationMatrix, const glm::vec3& SrcVertex, glm::vec3& DstVertex) const;

	private:

		// Start time of the key frame
		float StartTime;
		
		// Linking mode of the bones
		eBoneLinkingMode LinkingMode;

		// Weights of each bone
		std::vector<float> BoneWeights;

		// Deformation matrix of each bone
		std::vector<glm::mat4> BoneDeformationMatrices;

		// Vertex indices that each bone has influence on
		std::vector<std::vector<uint32_t>> BoneInfluencedVertexIndices;
	};
}

#endif // CXC_ANIMATIONKEYFRAME_H