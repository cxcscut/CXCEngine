#ifndef CXC_ANIMATIONCURVE_H
#define CXC_ANIMATIONCURVE_H

#include "Core/EngineTypes.h"

#define CURVENODE_TRANSFORM		"Transform"
#define CURVENODE_TRANSLATION	"T"
#define CURVENODE_ROTATION		"R"
#define CURVENODE_SCALING		"S"
#define CURVENODE_COMPONENT_X	"X"
#define CURVENODE_COMPONENT_Y	"Y"
#define CURVENODE_COMPONENT_Z	"Z"
#define CURVENODE_COLOR			"Color"
#define CURVENODE_COLOR_RED		CURVENODE_COMPONENT_X
#define CURVENODE_COLOR_GREEN	CURVENODE_COMPONENT_Y
#define CURVENODE_COLOR_BLUE	CURVENODE_COMPONENT_Z

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

	// What type the curve node is animating
	enum class eCurveNodeType : uint16_t
	{
		LocalTranslation,
		LocalRotation,
		LocalScaling,
		Color,
		LightIntensity,
		LightFog,
		LightOuterAngle,
		CameraFOV,
		CameraFOV_X,
		CameraFOV_Y,
		CameraOpticalCenterX,
		CameraOpticalCenterY,
		CameraRoll
	};

	class CXC_ENGINECORE_API AnimCurve final
	{
	public:
		AnimCurve();
		~AnimCurve();

	public:
		
		void SetCurveNodeName(const std::string& Name) { CurveNodeName = Name; }
		void AddKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);
		void DeleteKeyFrame(std::shared_ptr<AnimKeyFrame> pKeyFrame);

		std::string GetCurveNodeName() const { return CurveNodeName; }
		std::shared_ptr<AnimKeyFrame> GetKeyFrameByIndex(size_t Index);
		size_t GetKeyFrameCount() const { return KeyFrames.size(); }
		float GetStartTime() const;
		float GetEndTime() const;

	public:

		void Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);
		std::shared_ptr<AnimKeyFrame> FindCurrentKeyFrame(float CurrentTime);
		void SortKeyFramesByStartTime();

	private:

		// Name of the curve node
		std::string CurveNodeName;

		// Key frames the curve has
		std::vector<std::shared_ptr<AnimKeyFrame>> KeyFrames;
	};
}

#endif // CXC_ANIMATIONCURVE_H