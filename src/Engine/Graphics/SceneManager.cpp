#include "SceneManager.h"
#include "TextureManager.h"

#include "..\EngineFacade\EngineFacade.h"
namespace cxc {

	SceneManager::SceneManager()
		: m_ObjectMap(),TotalIndicesNum(0U)
	{
		m_pTextureMgr = TextureManager::GetInstance();
	}

	SceneManager::~SceneManager()
	{
		m_ObjectMap.clear();
	}

	void SceneManager::releaseBuffers() noexcept
	{
		if (VAO)
		{
			glDeleteVertexArrays(1,&VAO);
		}

		if (EBO)
		{
			glDeleteBuffers(1,&EBO);
		}

		if (VBO_P)
		{
			glDeleteBuffers(1,&VBO_P);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		}

		if (VBO_A)
		{
			glDeleteBuffers(1,&VBO_A);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
			glDisableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
			glDisableVertexAttribArray(static_cast<GLuint>(Location::COLOR_LOCATION));
		}
	}

	void SceneManager::InitBuffers() noexcept
	{
		std::vector<VertexAttri> m_VertexAttribs;
		std::vector<glm::vec3> m_VertexPos;
		std::vector<uint32_t> m_ElementBuffer;

		m_VertexPos.clear();
		m_VertexAttribs.clear();
		m_ElementBuffer.clear();
		uint32_t TotalVexNum = 0U;
		for (auto pObject : m_ObjectMap) {
			TotalVexNum += pObject.second->GetObjectVertexNum();
			auto model_map = pObject.second->GetModelMap();
			for (auto shape : model_map)
				TotalIndicesNum += shape.second->GetVertexIndices().size();
		}

		m_VertexPos.reserve(TotalVexNum);
		m_VertexAttribs.reserve(TotalVexNum);
		m_ElementBuffer.reserve(TotalIndicesNum);

		for(auto pObject: m_ObjectMap)
			pObject.second->GetObjectBuffers(m_VertexPos, m_VertexAttribs, m_ElementBuffer);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO_P);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_P);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexPos.size(), &m_VertexPos.front(), GL_DYNAMIC_DRAW);

		glGenBuffers(1, &VBO_A);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_A);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttri) *m_VertexAttribs.size(), &m_VertexAttribs.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_ElementBuffer.size(), &m_ElementBuffer.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_P);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, VBO_A);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttri), BUFFER_OFFSET(0)); // Texcoords

		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttri), BUFFER_OFFSET(sizeof(glm::vec2))); // Normal

		glEnableVertexAttribArray(static_cast<GLuint>(Location::COLOR_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::COLOR_LOCATION), 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttri), BUFFER_OFFSET(sizeof(glm::vec3) + sizeof(glm::vec2))); // Color
	}

	GLboolean SceneManager::CreateObject(const std::string &Object_name,const std::string &Object_file) noexcept
	{
		auto tmp_object = std::make_shared<Object3D >(Object_name);

		auto ret = tmp_object->LoadOBJFromFile(Object_file);
		if (!ret) return GL_FALSE;

		m_ObjectMap.insert(std::make_pair(Object_name, tmp_object));

		return GL_TRUE;
	}

	void SceneManager::AddObject(const std::string &ObjectName, const std::shared_ptr<Object3D > &ObjectPtr) noexcept
	{
		m_ObjectMap.insert(std::make_pair(ObjectName, ObjectPtr));
	}

	void SceneManager::DrawScene() noexcept
	{
		auto Engine = EngineFacade::GetInstance();

		auto SpriteProgramID = Engine->GetRendermanagerPtr()->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);

		Engine->BindCameraUniforms();
		Engine->GetRendermanagerPtr()->BindLightingUniforms();
		
		auto m_RendererManager = RendererManager::GetInstance();

		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

		GLuint M_MatrixID = glGetUniformLocation(SpriteProgramID, "M");
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &m_ModelMatrix[0][0]);

		// Updating coordinates position
		for (auto pObject : m_ObjectMap)
		{
			pObject.second->g_lock.lock();
			if (pObject.second->CheckStateChanged())
			{
				for (auto shape : pObject.second->GetModelMap())
				{
					if (shape.second->CheckStateChanged())
					{
						shape.second->UpDateCurrentPos();

						auto vertices = shape.second->GetVertexArray();
						auto offset = pObject.second->GetVertexSubscript(shape.second->GetModelName()) * sizeof(glm::vec3);
						glBindBuffer(GL_ARRAY_BUFFER,VBO_P);
						glBufferSubData(GL_ARRAY_BUFFER,offset,sizeof(glm::vec3) * vertices.size(),&vertices.front());

						shape.second->SetStateChanged(GL_FALSE);
					}
				}
				pObject.second->SetStateChanged(GL_FALSE);
			}
			pObject.second->g_lock.unlock();
		}

		glDrawElements(GL_TRIANGLES, TotalIndicesNum, GL_UNSIGNED_INT, (void*)0);

	}

	void SceneManager::DeleteObject(const std::string &sprite_name) noexcept
	{
		auto it = m_ObjectMap.find(sprite_name);
		if (it != m_ObjectMap.end())
			m_ObjectMap.erase(it);
	}

	std::shared_ptr<Object3D > SceneManager::GetObject3D(const std::string &sprite_name) const noexcept
	{
		auto it = m_ObjectMap.find(sprite_name);

		if (it != m_ObjectMap.end())
			return it->second;
		else
			return std::shared_ptr<Object3D >(nullptr);
	}

	const std::unordered_map<std::string, std::shared_ptr<Object3D >> &SceneManager::GetObjectMap() const noexcept
	{
		return m_ObjectMap;
	}

}
