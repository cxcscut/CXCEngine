#include "Animation/AnimLayer.h"
#include "Animation/AnimCurve.h"

namespace cxc
{
	AnimLayer::AnimLayer():
		Weight(0.0f)
	{

	}

	AnimLayer::~AnimLayer()
	{

	}

	std::shared_ptr<AnimCurve> AnimLayer::GetAnimCurveNode(size_t Index)
	{
		if (Index < AnimationCurveNodes.size())
		{
			return AnimationCurveNodes[Index];
		}
		else
			return nullptr;
	}

	void AnimLayer::AddAnimationCurve(std::shared_ptr<AnimCurve> pAnimCurve)
	{
		AnimationCurveNodes.push_back(pAnimCurve);
	}

	void AnimLayer::RemoveAnimationCurve(size_t Index)
	{
		if (Index < AnimationCurveNodes.size())
		{
			AnimationCurveNodes.erase(AnimationCurveNodes.begin() + Index);
		}

		// Shink to avoid holes
		AnimationCurveNodes.shrink_to_fit();
	}

	void AnimLayer::Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices)
	{
		for (auto pCurveNode : AnimationCurveNodes)
		{
			if (pCurveNode)
				pCurveNode->Evaluate(CurrentTime, PlayMode, SrcVertices, DstVertices);
		}
	}
}