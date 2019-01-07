#include "Geometry/DebugMesh.h"
#include "Geometry/SubMesh.h"

namespace cxc
{
	DebugMesh::DebugMesh():
		Mesh()
	{
		MeshName = "DebugMesh";
	}

	DebugMesh::DebugMesh(std::vector<glm::vec3>& Vertices)
		:Mesh()
	{
		MeshName = "DebugMesh";
		m_VertexCoords = Vertices;
	}

	DebugMesh::~DebugMesh()
	{

	}

	void DebugMesh::AllocateBuffers() noexcept
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		if (!m_VertexCoords.empty())
		{
			glGenBuffers(1, &m_VBO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexCoords.size(), &m_VertexCoords.front(), GL_STATIC_DRAW);
		}

		for (auto pSubMesh : SubMeshes)
		{
			if (pSubMesh)
			{
				pSubMesh->AllocateSubMeshEBO();
			}
		}
	}

	void DebugMesh::ReleaseBuffers() noexcept
	{
		if (glIsVertexArray(m_VAO))
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		for (auto pSubMesh : SubMeshes)
		{
			if (pSubMesh)
			{
				pSubMesh->ReleaseSubMeshEBO();
			}
		}

		if (glIsBuffer(m_VBO[0]))
		{
			glDeleteBuffers(1, &m_VBO[0]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		}
	}
}