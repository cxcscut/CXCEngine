#ifndef CXC_ANIMATIONCURVE_H
#define CXC_ANIMATIONCURVE_H

#include "Core/EngineTypes.h"

namespace cxc
{
	class AnimKeyFrame;
	enum class eAnimPlayMode : uint16_t;

	// Interpolation type
	enum class eInterpolationType : uint16_t
	{
		Lerp = 0, /* Linear interpolation */
		sLerp = 1  /* Spherical linear interpolation */,
		sQuard = 2  /* Spherical cubic spline interpolation */
	};

	class CXC_ENGINECORE_API AnimCurve final
	{
	public:
		AnimCurve();
		~AnimCurve();

	public:
		
		void SetNodeName(const std::string& Name) { NodeName = Name; }
		void AddKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);
		void DeleteKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);

		std::string GetNodeName() const { return NodeName; }
		std::shared_ptr<AnimKeyFrame> GetKeyFrameByIndex(size_t Index);
		size_t GetKeyFrameCount() const { return KeyFrames.size(); }
		float GetStartTime() const;
		float GetEndTime() const;

	public:

		void Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);
		std::shared_ptr<AnimKeyFrame> FindCurrentKeyFrame(float CurrentTime);
		void SortKeyFramesByStartTime();

	private:

		// Name of the node that the curve belongs to
		std::string NodeName;

		// Key frames the curve has
		std::vector<std::shared_ptr<AnimKeyFrame>> KeyFrames;
	};
}

#endif // CXC_ANIMATIONCURVE_H