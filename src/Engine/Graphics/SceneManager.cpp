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
		for (auto pObject : m_ObjectMap)
			pObject.second->releaseBuffers();
	}

	void SceneManager::InitBuffers() noexcept
	{
		for (auto pObject : m_ObjectMap)
			pObject.second->InitBuffers();
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
		for (auto pObject : m_ObjectMap)
			pObject.second->DrawObject();

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
