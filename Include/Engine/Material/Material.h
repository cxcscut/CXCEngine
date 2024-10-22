#ifndef CXC_MATERIAL_H
#define CXC_MATERIAL_H

#include "Core/EngineTypes.h"

namespace cxc
{
	class MaterialManager;
	class Texture2D;
	class Mesh;

	using MaterialDiffuseSubroutineInfo = struct {

		std::string SubroutineUniformName;

		std::string TexturedSubroutineName;

		std::string NonTexturedSubroutineName;
	};

	class CXC_ENGINECORE_API Material final
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
		std::weak_ptr<Mesh> OwnerObject;

		/* Texture2D */
		std::vector<std::shared_ptr<Texture2D>> pTextures;
	};
}

#endif // CXC_MATERIAL_H