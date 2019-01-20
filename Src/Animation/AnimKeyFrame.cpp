#include "Animation/AnimKeyFrame.h"

namespace cxc
{
	AnimKeyFrame::AnimKeyFrame(float KeyTime, float Value)
		: FrameTime(KeyTime), KeyValue(Value)
	{
	}

	AnimKeyFrame::~AnimKeyFrame()
	{

	}

	void AnimKeyFrame::Evaluate(const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices)
	{
		
	}

	void AnimKeyFrame::DeformVertex(const glm::mat4 DeformationMatrix, const glm::vec3& SrcVertex, glm::vec3& DstVertex) const
	{
		glm::vec4 HomoSrcVertex = glm::vec4(SrcVertex, 1.0f);
		auto HomoDstVertex = DeformationMatrix * HomoSrcVertex;

		DstVertex = glm::vec3(HomoDstVertex.x, HomoDstVertex.y, HomoDstVertex.z);
	}
}