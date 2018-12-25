#include "Animation/AnimKeyFrame.h"

namespace cxc
{
	AnimKeyFrame::AnimKeyFrame(float Time, eBoneLinkingMode LinkMode,
		const std::vector<float>& Weights,
		const std::vector<glm::mat4>& DeformationMatrices,
		const std::vector<std::vector<uint32_t>>& InfluencedIndices)
	{
		StartTime = Time;
		LinkingMode = LinkMode;
		BoneWeights = Weights;
		BoneDeformationMatrices = DeformationMatrices;
		BoneInfluencedVertexIndices = InfluencedIndices;
	}

	AnimKeyFrame::~AnimKeyFrame()
	{

	}

	void AnimKeyFrame::Evaluate(const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices)
	{
		// Deform each vertex by transform matrix and weight
		size_t VerticesCount = SrcVertices.size();
		for (size_t VertexIndex = 0; VertexIndex < VerticesCount; ++VertexIndex)
		{
			glm::vec3 SrcVertex = SrcVertices[VertexIndex];
			glm::vec3 DstVertex;
			float Weight = BoneWeights[VertexIndex];

			if (Weight > 1e-6)
			{
				DeformVertex(BoneDeformationMatrices[VertexIndex], SrcVertex, DstVertex);
				if (LinkingMode == eBoneLinkingMode::Normalized)
				{
					// In the normalized link mode, a vertex is always totally influenced by the links
					DstVertex /= Weight;
				}
				else if (LinkingMode == eBoneLinkingMode::TotalOne)
				{
					// In the total 1 link mode, a vertex can be partially influenced by the links
					SrcVertex *= (1.0f - Weight);
					DstVertex += SrcVertex;
				}

				DstVertices[VertexIndex] = DstVertex;
			}
		}
	}

	void AnimKeyFrame::DeformVertex(const glm::mat4 DeformationMatrix, const glm::vec3& SrcVertex, glm::vec3& DstVertex) const
	{
		glm::vec4 HomoSrcVertex = glm::vec4(SrcVertex, 1.0f);
		auto HomoDstVertex = DeformationMatrix * HomoSrcVertex;

		DstVertex = glm::vec3(HomoDstVertex.x, HomoDstVertex.y, HomoDstVertex.z);
	}
}