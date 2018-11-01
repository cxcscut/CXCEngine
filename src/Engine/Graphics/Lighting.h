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

	enum class eInteractionType : uint16_t {
		Dynamic,
		Static
	};

	class BaseLighting
	{

		friend class FBXSDKUtil;

	public:

		BaseLighting();
		BaseLighting(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type, eInteractionType interactive);
		virtual ~BaseLighting();

	public:

		void SetLightPos(const glm::vec3 &pos) noexcept;
		void SetLightType(eLightType type) noexcept;
		void SetDirection(const glm::vec3 &dir) noexcept;

		glm::vec3 GetDirection() const noexcept;
		eLightType GetLightType() const noexcept;
		glm::vec3 GetLightPos() const noexcept;;

	private:

		std::string LightName;

		glm::vec3 LightColor;

		glm::vec3 LightPos;

		glm::vec3 LightDirection;

		double LightIntensity;

		eLightType LightType;

		eInteractionType Interactive;

		bool bCastLight;

		bool bCastShadow;

	};
	
	class DynamicLighting : public BaseLighting
	{
	public :

		DynamicLighting(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type);
		~DynamicLighting();

	public:

		void PreBakeLightMap() noexcept;
		GLuint GetLightMap() const noexcept;
		GLuint GetLightMapFBO() const noexcept;

	private:
	
		GLuint LightMapTexture;
		GLuint LightMapFBO;
	};
}
#endif // CXC_LIGHTING_H
