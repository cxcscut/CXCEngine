#include "Material.h"
#include "MaterialManager.h"
#include "Texture2D.h"
#include "Object3D.h"

namespace cxc
{
	Material::Material()
	{
		OwnerObject.reset();
	}

	Material::~Material()
	{

	}
}