#include "Scene/Mesh.h"
#include "Scene/Object3D.h"

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

	void Mesh::BindMaterial(GLuint ProgramID, const MaterialDiffuseSubroutineInfo& DiffuseModelInfo, std::vector<GLuint>& SubroutinesIndicesFS)
	{
		GLuint TexSamplerLocation = glGetUniformLocation(ProgramID, "Material.TexSampler");
		GLuint KaLocation = glGetUniformLocation(ProgramID, "Material.Ka");
		GLuint KdLocation = glGetUniformLocation(ProgramID, "Material.Kd");
		GLuint KsLocation = glGetUniformLocation(ProgramID, "Material.Ks");
		GLuint ShiniessLoc = glGetUniformLocation(ProgramID, "Material.Shiniess");

		bool bHasTexture = true;
		GLint DiffuseModelSubroutineLoc = glGetSubroutineUniformLocation(ProgramID, GL_FRAGMENT_SHADER, DiffuseModelInfo.SubroutineUniformName.c_str());

		if (pMaterial)
		{
			glUniform3f(KaLocation, pMaterial->AmbientFactor.x, pMaterial->AmbientFactor.y, pMaterial->AmbientFactor.z);
			glUniform3f(KdLocation, pMaterial->DiffuseFactor.x, pMaterial->DiffuseFactor.y, pMaterial->DiffuseFactor.z);
			glUniform3f(KsLocation, pMaterial->SpecularFactor.x, pMaterial->SpecularFactor.y, pMaterial->SpecularFactor.z);
			glUniform1f(ShiniessLoc, pMaterial->Shiniess);

			if (pMaterial->pTextures.size() > 0)
			{
				// Choose the texture diffuse subroutine
				GLuint TextureDiffuseSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, DiffuseModelInfo.TexturedSubroutineName.c_str());
				if (DiffuseModelSubroutineLoc >= 0)
				{
					SubroutinesIndicesFS[DiffuseModelSubroutineLoc] = TextureDiffuseSubroutineIndex;
				}
				else
				{
					std::cerr << "Failed to find the subroutine uniform : " << DiffuseModelInfo.SubroutineUniformName << std::endl;
				}

				glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::UserTextureUnit);
				glBindTexture(GL_TEXTURE_2D, pMaterial->pTextures[0]->GetTextureID());

				glUniform1i(TexSamplerLocation, (GLuint)TextureUnit::UserTextureUnit);
			}
			else
			{
				bHasTexture = false;
			}
		}
		else
		{
			bHasTexture = false;
		}

		if (!bHasTexture)
		{
			// Choose the non-texture diffuse subroutine
			GLuint NonTextureDiffuseSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, DiffuseModelInfo.NonTexturedSubroutineName.c_str());

			if (DiffuseModelSubroutineLoc >= 0)
			{
				SubroutinesIndicesFS[DiffuseModelSubroutineLoc] = NonTextureDiffuseSubroutineIndex;
			}
			else
			{
				std::cerr << "Failed to find the subroutine uniform : " << DiffuseModelInfo.SubroutineUniformName << std::endl;
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
		if (glIsBuffer(MeshEBO))
		{
			glDeleteBuffers(1, &MeshEBO);
		}
	}

	void Mesh::DrawMesh()
	{
		auto pOwner = pOwnerObject.lock();
		glBindVertexArray(pOwner->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshEBO);

		glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetVertexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetTexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords

		glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetNormalsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal

		// Note : the 4-th parameter of glDrawElements is the offset of EBO which must be sizeof(DataType) * number of indices
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glDisableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glDisableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
	}
}