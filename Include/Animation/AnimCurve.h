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
		AnimCurve(float Start, float End);
		~AnimCurve();

	public:
		
		void AddKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);
		void DeleteKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);

		std::shared_ptr<AnimKeyFrame> GetKeyFrameByIndex(size_t Index);
		size_t GetKeyFrameCount() const { return KeyFrames.size(); }
		float GetStartTime() const { return StartTime; }
		float GetEndTime() const { return EndTime; }

	public:

		void Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);

	private:

		std::shared_ptr<AnimKeyFrame> FindCurrentKeyFrame(float CurrentTime);
		void SortKeyFramesByStartTime();

	private:

		// Start time of the curve node;
		float StartTime;

		// End time of the curve node;
		float EndTime;

		// Key frames the curve has
		std::vector<std::shared_ptr<AnimKeyFrame>> KeyFrames;
	};
}

#endif // CXC_ANIMATIONCURVE_H