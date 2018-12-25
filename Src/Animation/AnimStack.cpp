#include "Animation/AnimStack.h"
#include "Animation/AnimLayer.h"

namespace cxc
{

	AnimStack::AnimStack(const std::string& Name) :
		AnimationStackName(Name), pCurrentActiveAnimLayer(nullptr)
	{
		
	}

	AnimStack::~AnimStack()
	{

	}

	void AnimStack::AddAnimationLayer(std::shared_ptr<AnimLayer> pAnimLayer)
	{
		AnimLayers.push_back(pAnimLayer);
	}

	void AnimStack::RemoveAnimationLayer(size_t Index)
	{
		if (Index < AnimLayers.size())
		{
			AnimLayers.erase(AnimLayers.begin() + Index);
		}

		// Shink to avoid holes
		AnimLayers.shrink_to_fit();
	}

	void AnimStack::Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices)
	{
		// Reset the DstVertices
		DstVertices = SrcVertices;

		if (pCurrentActiveAnimLayer)
		{
			pCurrentActiveAnimLayer->Evaluate(CurrentTime, PlayMode, SrcVertices, DstVertices);
		}
	}

	void AnimStack::SetCurrentActiveAnimLayer(size_t LayerIndex)
	{
		if (LayerIndex >= AnimLayers.size())
			pCurrentActiveAnimLayer = nullptr;
		else
			pCurrentActiveAnimLayer = AnimLayers[LayerIndex];
	}
}