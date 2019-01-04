#include "Geometry/SubMesh.h"
#include "Geometry/Mesh.h"
#include "Utilities/DebugLogger.h"

namespace cxc {

	SubMesh::SubMesh():
		SubMeshEBO(0),
		ShadingMode(eShadingMode::FillMode)
	{
		pOwnerMesh.reset();
	}

	SubMesh::SubMesh(const std::vector<uint32_t>& indices):
		SubMesh()
	{
		Indices = indices;
	}

	SubMesh::~SubMesh()
	{

	}

	void SubMesh::SetOwnerObject(std::shared_ptr<Mesh> NewOwnerObject)
	{
		pOwnerMesh = NewOwnerObject;
	}

	std::shared_ptr<Mesh> SubMesh::GetOwnerMesh()
	{
		if (pOwnerMesh.expired())
			return nullptr;
		else
			return pOwnerMesh.lock();
	}

	void SubMesh::BindMaterial(GLuint ProgramID, const MaterialDiffuseSubroutineInfo& DiffuseModelInfo, std::vector<GLuint>& SubroutinesIndicesFS)
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
					DEBUG_LOG(eLogType::Error, "SubMesh::BindMaterial, Failed to find the subroutine uniform : " + DiffuseModelInfo.SubroutineUniformName + '\n');
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
				DEBUG_LOG(eLogType::Error, "SubMesh::BindMaterial, Failed to find the subroutine uniform : " + DiffuseModelInfo.SubroutineUniformName + '\n');
			}
		}
	}

	void SubMesh::AllocateSubMeshEBO()
	{
		if (!Indices.empty())
		{
			glGenBuffers(1, &SubMeshEBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SubMeshEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * Indices.size(), &Indices.front(), GL_STATIC_DRAW);
		}
	}

	void SubMesh::ReleaseSubMeshEBO()
	{
		if (glIsBuffer(SubMeshEBO))
		{
			glDeleteBuffers(1, &SubMeshEBO);
		}
	}

	void SubMesh::DrawSubMesh()
	{
		auto pOwner = pOwnerMesh.lock();
		glBindVertexArray(pOwner->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SubMeshEBO);

		if (glIsBuffer(pOwner->GetVertexCoordsVBO()) == GL_TRUE)
		{
			glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetVertexCoordsVBO());
			glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
			glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position
		}

		if (glIsBuffer(pOwner->GetTexCoordsVBO()) == GL_TRUE)
		{
			glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetTexCoordsVBO());
			glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
			glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords
		}

		if (glIsBuffer(pOwner->GetNormalsVBO()) == GL_TRUE)
		{
			glBindBuffer(GL_ARRAY_BUFFER, pOwner->GetNormalsVBO());
			glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
			glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal
		}

		// Set the shading mode the submesh
		// Wanring : OpenGL 3.1 and plus only receives GL_FRONT_AND_BACK
		switch (ShadingMode)
		{
		case eShadingMode::PointMode:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case eShadingMode::FillMode:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case eShadingMode::WireframeMode:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			break;
		default:
			break;
		}

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