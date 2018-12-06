#ifndef CXC_MATERIAL_H
#define CXC_MATERIAL_H

#include "General/DefineTypes.h"

namespace cxc
{
	class MaterialManager;
	class Texture2D;
	class Object3D;

	using MaterialDiffuseSubroutineInfo = struct {

		std::string SubroutineUniformName;

		std::string TexturedSubroutineName;

		std::string NonTexturedSubroutineName;
	};

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
		std::vector<std::shared_ptr<Texture2D>> pTextures;
	};
}

#endif // CXC_MATERIAL_H