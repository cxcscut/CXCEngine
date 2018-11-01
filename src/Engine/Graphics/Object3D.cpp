#include "Object3D.h"
#include "TextureManager.h"
#include "Shape.h"
#include "MaterialManager.h"

#ifdef WIN32

#include "..\Common\FileHelper.h"
#include "..\EngineFacade\EngineFacade.h"
#include "..\Graphics\RendererManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\Libraries\Image_loader\stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\Libraries\TinyObjLoader\tiny_obj_loader.h"

#else

#include "../Common/FileHelper.h"
#include "../Graphics/RendererManager.h"
#include "../EngineFacade/EngineFacade.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Libraries/Image_loader/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Libraries/TinyObjLoader/tiny_obj_loader.h"

#endif // WIN32

namespace cxc {

	Object3D::Object3D() :
		ObjectName(""), m_ModelMap(), Materials(), 
		isLoaded(false), m_ObjectTree(), stateChanged(GL_FALSE), enable(GL_TRUE),
		isKinematics(false)
	{

		auto max_float = std::numeric_limits<float>::max();

		// Initialize MinCoords with maximum float limit, MaxCoords with minimum float limit
		MinCoords = glm::vec3(max_float, max_float, max_float);
		MaxCoords = glm::vec3(-max_float, -max_float, -max_float);

	}

	Object3D::Object3D(const std::string &sprite_name)
		: Object3D()
	{
		ObjectName = sprite_name;
	}

	Object3D::Object3D(const std::string &Object_name, const std::string &filename, const std::string &_tag, GLboolean _enable)
		: Object3D()
	{
		ObjectName = Object_name;
		FileName = filename;
		tag = _tag;
		enable = _enable;
	}

	Object3D ::~Object3D()
	{

	}

	void Object3D::SetLoaded() noexcept {
		isLoaded = true;
	}

	GLboolean Object3D::CheckStateChanged() const noexcept
	{
		return stateChanged;
	}

	void Object3D::SetStateChanged(GLboolean state) noexcept
	{
		stateChanged = state;
	}

	std::vector<std::string> Object3D::GetModelNames() const noexcept
	{
		std::vector<std::string> ret;
		ret.reserve(m_ModelMap.size());

		for (auto it : m_ModelMap)
			ret.emplace_back(it.first);

		return ret;
	}

	std::unordered_map<std::string, std::shared_ptr<Shape>> &Object3D::GetModelMap() noexcept
	{
		return m_ModelMap;
	}

	void Object3D::ComputeNormal(glm::vec3 &normal, const glm::vec3 &vertex1, const glm::vec3 &vertex2, const glm::vec3 &vertex3) const noexcept
	{
		glm::vec3 vector12 = vertex2 - vertex1;
		glm::vec3 vector13 = vertex3 - vertex1;

		normal = glm::normalize(glm::cross(vector12,vector13));

	}

	/*
	void Object3D::CalculateSizeVector() noexcept
	{
		float MaxExtent = 0.5f * (MaxCoords.x - MinCoords.x);

		if (MaxExtent < 0.5f * (MaxCoords.y - MinCoords.y))
			MaxExtent = 0.5f * (MaxCoords.y - MinCoords.y);

		if (MaxExtent < 0.5f *(MaxCoords.z - MinCoords.z))
			MaxExtent = 0.5f * (MaxCoords.z - MinCoords.z);

		auto scale_factor = 1.0f / MaxExtent;

		ScaleVector = glm::vec3(scale_factor, scale_factor, scale_factor);
		CenterizeVector = glm::vec3(
			-0.5f * (MaxCoords.x - MinCoords.x),
			-0.5f * (MaxCoords.y - MinCoords.y),
			-0.5f * (MaxCoords.z - MinCoords.z)
		);
	}
*/
	void Object3D::UpdateBoundaryCoords(const glm::vec3 &pos) noexcept
	{
		// Update MaxCoords
		MaxCoords.x = std::fmax(pos.x, MaxCoords.x);
		MaxCoords.y = std::fmax(pos.y, MaxCoords.y);
		MaxCoords.z = std::fmax(pos.z, MaxCoords.z);

		// Update MinCoords
		MinCoords.x = std::fmin(pos.x, MinCoords.x);
		MinCoords.y = std::fmin(pos.y, MinCoords.y);
		MinCoords.z = std::fmin(pos.z, MinCoords.z);
	}

	void Object3D::InitializeRigidBodies(dWorldID world,dSpaceID space) noexcept
	{

		for (auto shape : m_ModelMap) {
			shape.second->createRigidBody(world,reinterpret_cast<void*>(shape.second.get()));
			shape.second->addCollider(space, shape.second->GetVertexArray(), shape.second->GetVertexIndices());

			if (isKinematics)
				dBodySetKinematic(shape.second->GetBodyID());
		}
	}

	void Object3D::IndexVertex(std::map<VertexIndexPacket, uint32_t> &VertexIndexMap,
		const glm::vec3 &geo_normal,const VertexIndexPacket &vertex,
		uint32_t &subindex, Shape *drawobject,const tinyobj::attrib_t &attrib)
	{
		auto it = VertexIndexMap.find(vertex);
		if (it == VertexIndexMap.end())
		{
			VertexIndexMap[vertex] = subindex;
			drawobject->m_VertexCoords.emplace_back(vertex.position);
			drawobject->m_VertexIndices.emplace_back(subindex);
			drawobject->m_TexCoords.emplace_back(vertex.texcoords);
			drawobject->m_VertexNormals.emplace_back(vertex.normal);
			subindex++;

			this->UpdateBoundaryCoords(vertex.position);
		}
		else
		{
			drawobject->m_VertexIndices.emplace_back(it->second);
		}

	}

	GLboolean Object3D::LoadOBJFromFile(const std::string &filename)
	{
		std::vector<tinyobj::material_t> materials;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<glm::vec3> g_vertex_normal;

		auto MaterialMgr = MaterialManager::GetInstance();
		auto TextureMgr = TextureManager::GetInstance();
		auto FileHelper = FileHelper::GetInstance();

		std::string base_dir = FileHelper->GetBaseDirectory(filename);

#ifdef _WIN32
		base_dir += "\\";
#else
		base_dir += "/";
#endif

		std::string err;
		GLboolean ret = tinyobj::LoadObj(&attrib,&shapes,&materials,&err,filename.c_str(),base_dir.c_str());

		if (attrib.vertices.empty())
			return GL_FALSE;

		if (!err.empty())
			std::cerr << err << std::endl;

		if (!ret)
		{
			std::cerr << "Failed to load " << filename << std::endl;
			return GL_FALSE;
		}

		// Append default material
		materials.push_back(tinyobj::material_t());

		// load diffuse texture
		for (size_t m = 0; m < materials.size(); ++m)
		{
			tinyobj::material_t *mp = &materials[m];

			// Create material instance
			auto pMaterialTmp = std::make_shared<Material>();
			pMaterialTmp->OwnerObject = shared_from_this();
			pMaterialTmp->MaterialName = mp->name;
			pMaterialTmp->AmbientFactor = glm::vec3(mp->ambient[0], mp->ambient[1], mp->ambient[2]);
			pMaterialTmp->DiffuseFactor = glm::vec3(mp->diffuse[0], mp->diffuse[1], mp->diffuse[2]);
			pMaterialTmp->SpecularFactor = glm::vec3(mp->specular[0], mp->specular[1], mp->specular[2]);

			if (mp->diffuse_texname.length() > 0)
			{
				// only load the texture if it is not loaded
				if (!TextureMgr->IsTextureExist(mp->diffuse_texname))
				{
					std::string texture_filename = mp->diffuse_texname;
					if (!FileHelper->FileIsExists(texture_filename))
					{
						// Append the base dir
						texture_filename = base_dir + mp->diffuse_texname;
						if (!FileHelper->FileIsExists(texture_filename))
						{
							std::cerr << "Unable to find file " << mp->diffuse_texname << std::endl;
							return GL_FALSE;
						}
						else
						{
							auto pNewTexture = TextureMgr->LoadTexture(mp->diffuse_texname, texture_filename);
							if (!pNewTexture)
							{
								std::cerr << "Failed to load the texture" << std::endl;
								return GL_FALSE;
							}

							pMaterialTmp->pTexture = pNewTexture;
						}
					}

					MaterialMgr->addMaterial(pMaterialTmp);
				}
			}
		}

		g_vertex_normal.reserve(attrib.vertices.size() / 3);

		// compute vertex normal
		if (attrib.normals.empty())
		{
			// Initialize vertex normal
			for (size_t i = 0; i < attrib.vertices.size() / 3; ++i)
				g_vertex_normal.push_back(glm::vec3(0.0f,0.0f,0.0f));

			// iteration on each shape
			for (size_t s = 0; s < shapes.size(); ++s)
			{

				// iteration on each face
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
				{
					glm::vec3 normal;

					// vertex index 0,1,2
					auto idx0 = shapes[s].mesh.indices[3 * f + 0].vertex_index;
					auto idx1 = shapes[s].mesh.indices[3 * f + 1].vertex_index;
					auto idx2 = shapes[s].mesh.indices[3 * f + 2].vertex_index;

					// vertex coordinates 0,1,2
					glm::vec3 vertex0 = glm::vec3(attrib.vertices[3 * idx0 + 0], attrib.vertices[3 * idx0 + 1], attrib.vertices[3 * idx0 + 2]);
					glm::vec3 vertex1 = glm::vec3(attrib.vertices[3 * idx1 + 0], attrib.vertices[3 * idx1 + 1], attrib.vertices[3 * idx1 + 2]);
					glm::vec3 vertex2 = glm::vec3(attrib.vertices[3 * idx2 + 0], attrib.vertices[3 * idx2 + 1], attrib.vertices[3 * idx2 + 2]);

					ComputeNormal(normal,vertex0,vertex1,vertex2);

					// Update normal arrays
					g_vertex_normal[idx0] += normal;
					g_vertex_normal[idx1] += normal;
					g_vertex_normal[idx2] += normal;
				}
			}

			for (auto &it : g_vertex_normal)
				it = glm::normalize(it);
		}

		// processing on each shape or Model
		for (size_t s = 0; s < shapes.size(); ++s)
		{
			auto drawobject = std::make_shared<Shape>();

			drawobject->SetTag(tag);

			drawobject->m_VertexCoords.reserve(shapes[s].mesh.indices.size());
			drawobject->m_VertexIndices.reserve(shapes[s].mesh.indices.size());
			drawobject->m_TexCoords.reserve(shapes[s].mesh.indices.size());

			drawobject->m_ModelName = shapes[s].name;

			std::map<VertexIndexPacket,uint32_t> vertex_index;
			uint32_t vertex_num = 0;

			drawobject->FragmentType = shapes[s].mesh.num_face_vertices[0];

			// processing on each face
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
			{
				// vertex position 0,1,2
				auto idx0 = shapes[s].mesh.indices[3 * f + 0];
				auto idx1 = shapes[s].mesh.indices[3 * f + 1];
				auto idx2 = shapes[s].mesh.indices[3 * f + 2];

				int current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					// Default material is added to the last item in `materials`.
					current_material_id = materials.size() - 1;
				}

				glm::vec2 texcoord0, texcoord1, texcoord2;
				glm::vec3 normal0, normal1, normal2;
				glm::vec3 geometric_normal;
				glm::vec3 vertex_tmp0, vertex_tmp1, vertex_tmp2;

				vertex_tmp0 = glm::vec3(attrib.vertices[3 * idx0.vertex_index + 0], attrib.vertices[3 * idx0.vertex_index + 1], attrib.vertices[3 * idx0.vertex_index + 2]);
				vertex_tmp1 = glm::vec3(attrib.vertices[3 * idx1.vertex_index + 0], attrib.vertices[3 * idx1.vertex_index + 1], attrib.vertices[3 * idx1.vertex_index + 2]);
				vertex_tmp2 = glm::vec3(attrib.vertices[3 * idx2.vertex_index + 0], attrib.vertices[3 * idx2.vertex_index + 1], attrib.vertices[3 * idx2.vertex_index + 2]);

				// texcoords 0,1
				if (attrib.texcoords.size()) {
					if (idx0.texcoord_index >= 0)
						texcoord0 = glm::vec2(attrib.texcoords[2 * idx0.texcoord_index + 0], attrib.texcoords[2 * idx0.texcoord_index + 1]);
					else
						texcoord0 = glm::vec2(0.0f, 0.0f);

					if (idx1.texcoord_index >= 0)
						texcoord1 = glm::vec2(attrib.texcoords[2 * idx1.texcoord_index + 0], attrib.texcoords[2 * idx1.texcoord_index + 1]);
					else
						texcoord1 = glm::vec2(0.0f, 0.0f);

					if (idx2.texcoord_index >= 0)
						texcoord2 = glm::vec2(attrib.texcoords[2 * idx2.texcoord_index + 0], attrib.texcoords[2 * idx2.texcoord_index + 1]);
					else
						texcoord2 = glm::vec2(0.0f, 0.0f);

				}
				else {
					texcoord0 = glm::vec2(0.0f, 0.0f);
					texcoord1 = glm::vec2(0.0f, 0.0f);
					texcoord2 = glm::vec2(0.0f, 0.0f);
				}

				// normal 0,1,2
				if (attrib.normals.size()) {
					if(idx0.normal_index >= 0)
						normal0 = glm::vec3(attrib.normals[3 * idx0.normal_index + 0], attrib.normals[3 * idx0.normal_index + 1], attrib.normals[3 * idx0.normal_index + 2]);
					else
						normal0 = g_vertex_normal[idx0.vertex_index];

					if (idx1.normal_index >= 0)
						normal1 = glm::vec3(attrib.normals[3 * idx1.normal_index + 0], attrib.normals[3 * idx1.normal_index + 1], attrib.normals[3 * idx1.normal_index + 2]);
					else
						normal1 = g_vertex_normal[idx1.vertex_index];

					if (idx2.normal_index >= 0)
						normal2 = glm::vec3(attrib.normals[3 * idx2.normal_index + 0], attrib.normals[3 * idx2.normal_index + 1], attrib.normals[3 * idx2.normal_index + 2]);
					else
						normal2 = g_vertex_normal[idx1.vertex_index];
				}
				else
				{
					normal0 = g_vertex_normal[idx0.vertex_index];
					normal1 = g_vertex_normal[idx1.vertex_index];
					normal2 = g_vertex_normal[idx2.vertex_index];
				}
				// Compute geometric normal per face
				ComputeNormal(geometric_normal,vertex_tmp0,vertex_tmp1,vertex_tmp2);

				// vertex packet for each vertex 0,1,2
				VertexIndexPacket vertex0(vertex_tmp0, normal0, texcoord0);
				VertexIndexPacket vertex1(vertex_tmp1, normal2, texcoord1);
				VertexIndexPacket vertex2(vertex_tmp2, normal2, texcoord2);

				// Create vertex indices
				IndexVertex(vertex_index,geometric_normal,vertex0,vertex_num,drawobject.get(),attrib);
				IndexVertex(vertex_index,geometric_normal,vertex1,vertex_num,drawobject.get(),attrib);
				IndexVertex(vertex_index,geometric_normal,vertex2,vertex_num,drawobject.get(),attrib);

			} // Loop on indices

			// Compute vertex color per face
			auto face_num = shapes[s].mesh.num_face_vertices.size();

			// Compute colors
			for (size_t f = 0; f < face_num; ++f)
			{

				auto current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					// Default material is added to the last item in `materials`.
					current_material_id = materials.size() - 1;
				}

				glm::vec3 normal0,normal1,normal2;

				normal0 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 0]];
				normal1 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 1]];
				normal2 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 2]];

			}

			drawobject->Num_of_faces = shapes[s].mesh.num_face_vertices.size();
			drawobject->m_MaterialIDs.reserve(drawobject->Num_of_faces);

			drawobject->ComputeCenterPoint();

			// do not support texturing pre-face
			bool bNeedCreateDefaultMaterial = false;
			if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s)
			{
				auto i = shapes[s].mesh.material_ids[s];
				if (i >= 0)
				{
					Materials.insert(std::make_pair(shapes[s].name, materials[i].name));

					auto MaterialIter = MaterialMgr->CachedMaterials.find(materials[i].name);
					if (MaterialIter != MaterialMgr->CachedMaterials.end())
					{
						drawobject->pMaterial = MaterialIter->second;
					}
				}
				else
				{
					Materials.insert(std::make_pair(shapes[s].name, materials.back().name));
					bNeedCreateDefaultMaterial = true;
				}
			}
			else
			{
				assert(materials.size() > 0);
				Materials.insert(std::make_pair(shapes[s].name, materials.back().name));
				bNeedCreateDefaultMaterial = true;
			}

			if (bNeedCreateDefaultMaterial)
			{
				// Create default material class
				auto pMaterialDefault = std::make_shared<Material>();
				pMaterialDefault->OwnerObject = shared_from_this();
				pMaterialDefault->MaterialName = materials.back().name;
				pMaterialDefault->AmbientFactor = glm::vec3(materials.back().ambient[0], materials.back().ambient[1], materials.back().ambient[2]);
				pMaterialDefault->DiffuseFactor = glm::vec3(materials.back().diffuse[0], materials.back().diffuse[1], materials.back().diffuse[2]);
				pMaterialDefault->SpecularFactor = glm::vec3(materials.back().specular[0], materials.back().specular[1], materials.back().specular[2]);

				MaterialMgr->addMaterial(pMaterialDefault);
				drawobject->pMaterial = pMaterialDefault;
			}

			m_ModelMap.insert(std::make_pair(shapes[s].name,std::move(drawobject)));

		} // shapes

		ComputeCenterPos();

		// Update AABB bounding box
		AABB.max = MaxCoords;
		AABB.min = MinCoords;

		/*
		// Calculate model matrix for each shapes
		this->CalculateSizeVector();

		for (auto &shape : m_ModelMap)
			shape.second->CalculateReposMatrix(ScaleVector,CenterizeVector);
		*/
		this->isLoaded = true;

		return GL_TRUE;
	} // LoadObjFile() function

	const std::string &Object3D ::GetObjectName() const noexcept
	{
		return ObjectName;
	}

	void Object3D::ComputeCenterPos() noexcept
	{
		size_t count = 0U;
		glm::vec3 pos;
		for (auto shape : m_ModelMap)
		{
			count++;
			shape.second->ComputeCenterPoint();
			pos += shape.second->GetCenterPos();
		}

		if (count)
			CenterCoords = glm::vec3({pos.x/count,pos.y/count,pos.z/count});
		else
			CenterCoords = pos;

	}

	bool Object3D ::AddRoot(const std::string &ModelName) noexcept
	{
		if (!this->isLoaded)
			return false;

		auto it = m_ModelMap.find(ModelName);
		if (it != m_ModelMap.end())
		{
			auto ModelPtr = it->second;
			if (ModelPtr->m_MyPtr == nullptr)
			{
				auto ObjectNodePtr = std::make_shared<ObjectTree>(ModelPtr);

				// Memory allocation failed
				if (!ObjectNodePtr)
					return false;

				ModelPtr->m_MyPtr = ObjectNodePtr;

				m_ObjectTree.emplace_back(ObjectNodePtr);

				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	bool Object3D ::AddChild(const std::string &RootModelName, const std::string &ChildModelName) noexcept
	{
		if (!this->isLoaded)
			return false;

		auto ModelPtr = GetModelByName(RootModelName);
		if (!ModelPtr)
			return false;

		auto RootNodePtr = ModelPtr->m_MyPtr;
		if (!RootNodePtr)
			return false;

		auto ChildModelPtr = GetModelByName(ChildModelName);
		if (!ChildModelPtr)
			return false;

		auto ObjectNodePtr = std::make_shared<ObjectTree>(ChildModelPtr);
		if (!ObjectNodePtr)
			return false;

		ChildModelPtr->m_MyPtr = ObjectNodePtr;

		RootNodePtr->addChildNode(ObjectNodePtr);

		return true;

	}

	bool Object3D ::FindChild(const std::string &RootModelName, const std::string &TargetModelName, std::shared_ptr<ObjectTree> &RetModelPtr) noexcept
	{

		RetModelPtr = nullptr;

		// Check if object is loaded
		if (!this->isLoaded)
			return false;

		// Check if root model exists
		auto ModelPtr = GetModelByName(RootModelName);
		if (!ModelPtr)
			return false;

		// Check if child node exists
		auto RootNodePtr = ModelPtr->m_MyPtr;
		if (!RootNodePtr)
			return false;

		auto ChildNodePtr = GetModelByName(TargetModelName);
		if (!ChildNodePtr)
			return false;

		return RootNodePtr->findChildNode(ChildNodePtr,RetModelPtr);
	}

	std::vector<std::shared_ptr<ObjectTree>> &Object3D ::GetObjectTreePtr() noexcept
	{
		return m_ObjectTree;
	}

	std::shared_ptr<Shape> Object3D ::GetModelByName(const std::string &ModelName) const noexcept
	{
		auto it = m_ModelMap.find(ModelName);
		if (it == m_ModelMap.end())
			return std::shared_ptr<Shape>(nullptr);
		else
			return it->second;
	}

	void Object3D::Translation(const glm::vec3 &TranslationVector) noexcept
	{
		for (auto shape : m_ModelMap)
			shape.second->Translate(TranslationVector);
	}

	void Object3D::Rotation(const glm::vec3 &RotationAxis, float Degree) noexcept
	{
		for (auto shape : m_ModelMap)
			shape.second->Rotate(RotationAxis,Degree);
	}

	void Object3D ::Translation(const std::string &ModelName, const glm::vec3 &TranslationVector) noexcept
	{

		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		// Perform translation on root node
		ModelPtr->Translate(TranslationVector);

		SetStateChanged(GL_TRUE);

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// Perform translation on children node
		for (auto &it : ObjectNodePtr->children)
			Translation(it->root->GetModelName(),TranslationVector);

	}

	void Object3D ::Rotation(const std::string &ModelName, const glm::vec3 &RotationAxis, float Degree) noexcept
	{

		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;
		// Perform rotation on root node
		ModelPtr->Rotate(RotationAxis,Degree);

		SetStateChanged(GL_TRUE);

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// Perform rotation on children node
		for (auto &it : ObjectNodePtr->children)
			Rotation(it->root->GetModelName(),RotationAxis,Degree);

	}

	bool Object3D ::CheckCompoent(const std::string &CompoentName) const noexcept
	{
		if (!this->isLoaded)
			return false;

		auto piter = m_ModelMap.cbegin();
		for (; piter != m_ModelMap.cend(); ++piter)
			if (piter->first == CompoentName)
				return true;

		return false;
	}

	uint32_t Object3D::GetVertexSubscript(const std::string &shape_name) noexcept
	{
		return m_VertexSubscript[shape_name];
	}

	void Object3D ::SetObjectName(const std::string &name) noexcept
	{
		ObjectName = name;
	}

	bool Object3D ::CheckLoadingStatus() const noexcept
	{
		return isLoaded;
	}

	uint32_t Object3D::GetObjectVertexNum() const noexcept
	{
		uint32_t vertexNum = 0;
		for (auto shape : m_ModelMap)
			vertexNum += shape.second->GetVertexNum();

		return vertexNum;
	}


	glm::vec3 Object3D ::CalculateRotatedCoordinate(const glm::vec3 &original_vec, const glm::vec3 &start, const glm::vec3 &direction,float degree) const noexcept
	{

		auto Forward = glm::translate(glm::mat4(1.0f),-start);

		auto RotMatrix = glm::rotate(glm::mat4(1.0f),degree,direction);

		auto Backward= glm::translate(glm::mat4(1.0f),start);

		auto TransMatrix = Backward * RotMatrix * Forward;

		auto homo_original_vec = glm::vec4(original_vec.x,original_vec.y,original_vec.z,1);

		auto new_vec = TransMatrix * homo_original_vec;

		return glm::vec3(new_vec.x / new_vec.w, new_vec.y / new_vec.w, new_vec.z / new_vec.w);
	}

	void Object3D::InitBuffers() noexcept
	{
		for (auto shape : m_ModelMap)
		{
			shape.second->InitBuffers();
		}
	}

	void Object3D::ReleaseBuffers() noexcept
	{
		for (auto shape : m_ModelMap)
		{
			shape.second->ReleaseBuffers();
		}
	}

	void Object3D::UpdateMeshTransMatrix() noexcept
	{
		for (auto shape : m_ModelMap)
			shape.second->UpdateMeshTransform();
	}

	
	void Object3D::SetObjectGravityMode(int mode) noexcept
	{
		for (auto shape : m_ModelMap)
			shape.second->setGravityMode(mode);
	}

	void Object3D::RenderingTick() noexcept
	{
		for (auto pShape : m_ModelMap)
		{
			pShape.second->RenderingTick();
		}
	}

	void Object3D::DrawShadow(ShadowMapRender* pShadowRender) noexcept
	{
		for (auto pShape : m_ModelMap)
		{
			pShape.second->ShadowCastTick(pShadowRender);
		}
	}

	void Object3D::RotateWithArbitraryAxis(const std::string &ModelName, const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept
	{

		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 M = glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f),start),degree,direction),-start);

		// Perform rotation on root node
		// Set 3x3 Rotation matrix
		ModelPtr->setRotation((glm::mat3)M * ModelPtr->getRotation());

		auto P = ModelPtr->getPosition();

		// Set position vector
		ModelPtr->setPossition(
			M[0][0] * P.x + M[1][0] * P.y + M[2][0] * P.z + M[3][0],
			M[0][1] * P.x + M[1][1] * P.y + M[2][1] * P.z + M[3][1],
			M[0][2] * P.x + M[1][2] * P.y + M[2][2] * P.z + M[3][2]
		);

		SetStateChanged(GL_TRUE);

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// Perform rotation on children node
		for (auto &children : ObjectNodePtr->children)
			RotateWithArbitraryAxis(children->root->GetModelName(),start,direction,degree);

	}

	CXCRect3::CXCRect3(const glm::vec3 &_max, const glm::vec3 &_min) :max(_max), min(_min) {};

	CXCRect3::CXCRect3() :
		max(glm::vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())),
		min(glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()))
	{}

	CXCRect3::~CXCRect3()
	{}

	CXCRect3::CXCRect3(const CXCRect3 &other)
	{
		max = other.max;
		min = other.min;
	}

	CXCRect3 &CXCRect3::operator=(const CXCRect3 &other)
	{
		max = other.max;
		min = other.min;

		return *this;
	}

	bool CXCRect3::isContain(const CXCRect3 &rhs) const noexcept
	{
		return rhs.max.x <= max.x && rhs.min.x >= min.x &&
			rhs.max.y <= max.y && rhs.min.y >= min.y &&
			rhs.max.z <= max.z && rhs.min.z >= min.z;
	}

	bool CXCRect3::isIntersected(const CXCRect3 &other) const noexcept
	{
		return ((min.x >= other.min.x && min.x <= other.max.x) || (other.min.x >= min.x && other.min.x <= max.x)) &&
			((min.y >= other.min.y && min.y <= other.max.y) || (other.min.y >= min.y && other.min.y <= max.y)) &&
			((min.z >= other.min.z && min.z <= other.max.z) || (other.min.z >= min.z && other.min.z <= max.z));
	}
}

