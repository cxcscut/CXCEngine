#include "General/DefineTypes.h"

#ifndef CXC_LIGHTING_H
#define CXC_LIGHTING_H

namespace cxc {

	enum class eLightType : uint16_t {
		Directional,
		Spot,
		OmniDirectional,
		Area,
		Volumetric,
		InvalidType
	};

	enum class eLightAtteunationType : uint16_t {
		None = 0,
		Linear = 1,
		Quadratic = 2,
		Cubic = 3
	};

	class LightSource
	{

		friend class FBXSDKUtil;

	public:

		LightSource();
		LightSource(const std::string& Name, const glm::vec3 &pos, const glm::vec3 &TargetPos, float Intensity, eLightType type);
		virtual ~LightSource();

	public:

		void SetLightPos(float x, float y, float z) noexcept;
		void SetLightType(eLightType type) noexcept;
		void SetTargetPos(float x, float y, float z) noexcept;
		void SetIntensity(float NewIntensity) { LightIntensity = NewIntensity; }
		void SetLightColor(float r, float g, float b) { LightColor = glm::vec3(r, g, b); }
		void SetLightAtteunationType(eLightAtteunationType NewAtteunationType) { AtteunationType = NewAtteunationType; }

		eLightAtteunationType GetAtteunationType() const { return AtteunationType; }
		glm::vec3 GetTargetPos() const noexcept;
		eLightType GetLightType() const noexcept;
		glm::vec3 GetLightPos() const noexcept;
		const std::string& GetLightName() const { return LightName; }
		float GetIntensity() const { return LightIntensity; }
		glm::vec3 GetLightColor() const noexcept { return LightColor; }

	private:

		std::string LightName;

		glm::vec3 LightColor;

		glm::vec3 LightPos;

		glm::vec3 TargetPos;

		float LightIntensity;

		eLightType LightType;

		eLightAtteunationType AtteunationType;

		bool bCastLight;

		bool bCastShadow;

	};

}
#endif // CXC_LIGHTING_H
