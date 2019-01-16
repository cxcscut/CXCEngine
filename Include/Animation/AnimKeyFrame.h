#ifndef CXC_ANIMATIONKEYFRAME_H
#define CXC_ANIMATIONKEYFRAME_H

#include "Core/EngineTypes.h"

namespace cxc
{
	// Linking mode of each bones of the skeleton, all the linking mode must be the same
	enum class eBoneLinkingMode : uint16_t
	{
		Additive = 0,
		Normalized = 1,
		TotalOne = 2
	};

	class CXC_ENGINECORE_API AnimKeyFrame final
	{
	public:
		AnimKeyFrame(float Time, eBoneLinkingMode LinkMode,
			const std::vector<float>& Weights,
			const std::vector<glm::mat4>& DeformationMatrices,
			const std::vector<std::vector<uint32_t>>& InfluencedIndices);

		~AnimKeyFrame();

	public:

		float GetFrameTime() const { return FrameTime; }

	public:

		void Evaluate(const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);

	private:

		void DeformVertex(const glm::mat4 DeformationMatrix, const glm::vec3& SrcVertex, glm::vec3& DstVertex) const;

	private:

		// Start time of the key frame
		float FrameTime;
		
		// Value of the keyframe
		float Value;

	};
}

#endif // CXC_ANIMATIONKEYFRAME_H