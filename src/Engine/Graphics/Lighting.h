#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif

#ifndef CXC_LIGHTING_H
#define CXC_LIGHTING_H

namespace cxc {
	enum class LightType : uint16_t {
		Directional,
		Spot,
		Omni_Directional,
		InvalidType
	};

	enum class InteractionType : uint16_t {
		Dynamic,
		Static
	};

	class BaseLighting
	{

	public:

		BaseLighting(const glm::vec3 &pos, const glm::vec3 &dir,LightType type,InteractionType interactive);
		virtual ~BaseLighting();

	public:

		void SetLightPos(const glm::vec3 &pos) noexcept;
		void SetLightType(LightType type) noexcept;
		void SetDirection(const glm::vec3 &dir) noexcept;

		glm::vec3 GetDirection() const noexcept;
		LightType GetLightType() const noexcept;
		glm::vec3 GetLightPos() const noexcept;;

	private:

		glm::vec3 LightPos;
		glm::vec3 LightDirection;
		LightType Type;
		InteractionType Interactive;
	};

	class DynamicLighting : public BaseLighting
	{
	public :

		DynamicLighting(const glm::vec3 &pos, const glm::vec3 &dir, LightType type);
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
