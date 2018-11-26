#include "Mesh.h"

namespace cxc {

	Mesh::Mesh():
		MeshEBO(0)
	{
		pOwnerObject.reset();
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::SetOwnerObject(std::shared_ptr<Object3D> NewOwnerObject)
	{
		pOwnerObject = NewOwnerObject;
	}

	std::shared_ptr<Object3D> Mesh::GetOwnerObject()
	{
		if (pOwnerObject.expired())
			return nullptr;
		else
			return pOwnerObject.lock();
	}

	void Mesh::BindMaterial(GLuint KaLocation, GLuint KdLocation, GLuint KsLocation, GLuint TexSampler)
	{
		if (pMaterial)
		{
			glUniform3f(KaLocation, pMaterial->AmbientFactor.x, pMaterial->AmbientFactor.y, pMaterial->AmbientFactor.z);
			glUniform3f(KdLocation, pMaterial->DiffuseFactor.x, pMaterial->DiffuseFactor.y, pMaterial->DiffuseFactor.z);
			glUniform3f(KsLocation, pMaterial->SpecularFactor.x, pMaterial->SpecularFactor.y, pMaterial->SpecularFactor.z);

			if (pMaterial->pTextures.size() > 0)
			{
				glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::UserTextureUnit);
				glBindTexture(GL_TEXTURE_2D, pMaterial->pTextures[0]->GetTextureID());

				glUniform1i(TexSampler, (GLuint)TextureUnit::UserTextureUnit);
			}
		}
	}

	void Mesh::AllocateMeshEBO()
	{
		glGenBuffers(1, &MeshEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * Indices.size(), &Indices.front(), GL_STATIC_DRAW);
	}

	void Mesh::ReleaseMeshEBO()
	{
		if (MeshEBO)
		{
			glDeleteBuffers(1, &MeshEBO);
		}
	}

	void Mesh::DrawMesh()
	{
		// Note : the 4-th parameter of glDrawElements is the offset of EBO which must be sizeof(DataType) * number of indices
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}
}