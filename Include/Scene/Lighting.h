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
		void SetCutOffAngle(float OuterAngle) { CutOffAngle = OuterAngle; }
		void SetAreaRadius(float Radius) { AreaRadius = Radius; }

		eLightAtteunationType GetAtteunationType() const { return AtteunationType; }
		glm::vec3 GetTargetPos() const noexcept;
		eLightType GetLightType() const noexcept;
		glm::vec3 GetLightPos() const noexcept;
		const std::string& GetLightName() const { return LightName; }
		float GetIntensity() const { return LightIntensity; }
		glm::vec3 GetLightColor() const noexcept { return LightColor; }
		float GetCutOffAngle() const { return CutOffAngle; }
		float GetAreaRadius() const { return AreaRadius; }

	private:

		// Name of the light source
		std::string LightName;

		// Color of the light source
		glm::vec3 LightColor;

		// Position of the light source
		glm::vec3 LightPos;

		// Target position the light source is looking at
		glm::vec3 TargetPos;

		// Radius of the light area
		float AreaRadius;

		// Intensity of the light source
		float LightIntensity;

		// Type of the light source
		eLightType LightType;

		// Atteunation of the light source
		eLightAtteunationType AtteunationType;

		// Cutoff angle of the spotlight
		float CutOffAngle;

		// Whether the Light casts 
		bool bCastLight;

		// Whether the light casts shadows
		bool bCastShadow;

	};

}
#endif // CXC_LIGHTING_H
