#include "Animation/AnimLayer.h"
#include "Animation/AnimCurve.h"

namespace cxc
{
	AnimLayer::AnimLayer(const std::string& Name):
		Weight(0.0f), LayerName(Name)
	{

	}

	AnimLayer::~AnimLayer()
	{

	}

	std::shared_ptr<AnimCurve> AnimLayer::GetAnimCurveNode(size_t Index)
	{
		if (Index < AnimCurves.size())
		{
			return AnimCurves[Index];
		}
		else
			return nullptr;
	}

	void AnimLayer::AddAnimationCurve(std::shared_ptr<AnimCurve> pAnimCurve)
	{
		if(pAnimCurve != nullptr)
			AnimCurves.push_back(pAnimCurve);
	}

	void AnimLayer::RemoveAnimationCurve(size_t Index)
	{
		if (Index < AnimCurves.size())
		{
			AnimCurves.erase(AnimCurves.begin() + Index);
		}

		// Shink to avoid holes
		AnimCurves.shrink_to_fit();
	}

	void AnimLayer::Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices)
	{
		for (auto pCurveNode : AnimCurves)
		{
			if (pCurveNode)
				pCurveNode->Evaluate(CurrentTime, PlayMode, SrcVertices, DstVertices);
		}
	}
}