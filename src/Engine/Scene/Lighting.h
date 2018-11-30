#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif

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

	class LightSource
	{

		friend class FBXSDKUtil;

	public:

		LightSource();
		LightSource(const std::string& Name, const glm::vec3 &pos, const glm::vec3 &dir, float Intensity, eLightType type);
		virtual ~LightSource();

	public:

		void SetLightPos(float x, float y, float z) noexcept;
		void SetLightType(eLightType type) noexcept;
		void SetDirection(float x, float y, float z) noexcept;
		void SetIntensity(float NewIntensity) { LightIntensity = NewIntensity; }
		void SetLightColor(float r, float g, float b) { LightColor = glm::vec3(r, g, b); }

		glm::vec3 GetDirection() const noexcept;
		eLightType GetLightType() const noexcept;
		glm::vec3 GetLightPos() const noexcept;
		const std::string& GetLightName() const { return LightName; }
		float GetIntensity() const { return LightIntensity; }
		glm::vec3 GetLightColor() const noexcept { return LightColor; }

	private:

		std::string LightName;

		glm::vec3 LightColor;

		glm::vec3 LightPos;

		glm::vec3 LightDirection;

		float LightIntensity;

		eLightType LightType;

		bool bCastLight;

		bool bCastShadow;

	};

}
#endif // CXC_LIGHTING_H
