#ifndef CXC_MATERIAL_H
#define CXC_MATERIAL_H

#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif // WIN32

namespace cxc
{
	class MaterialManager;
	class Texture2D;
	class Object3D;

	class Material final
	{

	public:
		Material();
		Material(const std::string& MaterialName, const glm::vec3& Emissive, const glm::vec3& Ambient, const glm::vec3& Diffuse, const glm::vec3& Specular, float Shiniess);
		~Material();

	public:

		glm::vec3 EmissiveFactor;

		glm::vec3 AmbientFactor;
		
		glm::vec3 DiffuseFactor;

		glm::vec3 SpecularFactor;

		float Shiniess;

		std::string MaterialName;

		/* Object that own the material */
		std::weak_ptr<Object3D> OwnerObject;

		/* Texture2D */
		std::shared_ptr<Texture2D> pTexture;
	};
}

#endif // CXC_MATERIAL_H