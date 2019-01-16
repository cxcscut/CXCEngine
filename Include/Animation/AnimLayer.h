#ifndef CXC_ANIMATIONLAYER_H
#define CXC_ANIMATIONLAYER_H

#include "Core/EngineTypes.h"

namespace cxc
{
	class AnimCurve;
	enum class eAnimPlayMode : uint16_t;

	// Animation layer is a collection of animation curve nodes
	class CXC_ENGINECORE_API AnimLayer final
	{
	public:
		AnimLayer();
		~AnimLayer();

	public:

		std::shared_ptr<AnimCurve> GetAnimCurveNode(size_t Index);

	public:

		void AddAnimationCurve(std::shared_ptr<AnimCurve> pAnimCurve);
		void RemoveAnimationCurve(size_t Index);

	public:

		void Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);

	private:

		// Weight of the layer
		float Weight;

		// Animation curve nodes
		std::vector<std::shared_ptr<AnimCurve>> AnimCurves;

	};
}

#endif // CXC_ANIMATIONLAYER_H