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

	// How the keyframes are interpolated
	enum class eKeyInterpolationType : uint16_t
	{
		Invalid = 0,
		Constant = 1,
		Linear = 2,
		Cubic = 3
	};

	// Constant mode
	enum class eConstantMode : uint16_t
	{
		Invalid = 0,
		Standard = 1,
		Next = 2,
	};

	// Cubic mode
	enum class eCubicMode : uint16_t
	{
		Invalid = 0,
		Auto = 1,
		AutoBreak = 2,
		TCB = 3,
		User = 4,
		Break = 5,
		UserBreak = 6,
	};

	// TangentWVMode 
	enum class eTangentWVMode : uint16_t
	{
		Invalid = 0,
		None = 1,
		Right = 2,
		NextLeft = 3,
	};

	class CXC_ENGINECORE_API AnimKeyFrame final
	{
	public:
		AnimKeyFrame(float KeyTime, float Value);
		~AnimKeyFrame();

	public:

		float GetFrameTime() const { return FrameTime; }

	public:

		void Evaluate(const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);

	private:

		void DeformVertex(const glm::mat4 DeformationMatrix, const glm::vec3& SrcVertex, glm::vec3& DstVertex) const;

	private:

		// Interpolation type
		eKeyInterpolationType InterpolationType;

		// Constant mode
		eConstantMode ConstantMode;

		// Cubic mode
		eCubicMode CubicMode;

		// TangentWVMode
		eTangentWVMode TangentWVMode;

		// Start time of the key frame
		float FrameTime;
		
		// Value of the keyframe
		float KeyValue;

	};
}

#endif // CXC_ANIMATIONKEYFRAME_H