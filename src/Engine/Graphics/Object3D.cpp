#include "Object3D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\Libraries\Image_loader\stb_image.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include "..\Libraries\TinyObjLoader\tiny_obj_loader.h"

#include "..\Helper\FileHelper.h"
#include "..\EngineFacade\EngineFacade.h"

namespace cxc {

	Object3D::Object3D() :
		ObjectName(""), m_ModelMap(),m_Material(),indices_num(0U),
		TexSamplerHandle(0U),VAO(0U),EBO(0U),VBO_A(0U),VBO_P(0U),
		isLoaded(false),m_ObjectTree(),stateChanged(GL_FALSE)
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

	Object3D::Object3D(const std::string &Object_name, const std::string &filename)
	{
		ObjectName = Object_name;
		LoadOBJFromFile(filename);
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
			drawobject->m_GeometricNormal.emplace_back(geo_normal);
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

		auto tex_manager = TextureManager::GetInstance();
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

			if(mp->diffuse_texname.length() >0)
				// only load the texture if it is not loaded
				if (tex_manager->m_TextureMap.find(mp->diffuse_texname) == tex_manager->m_TextureMap.end())
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
					}

					tex_manager->addTexture(mp->diffuse_texname, texture_filename);
					m_TexNames.emplace_back(mp->diffuse_texname);
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

			drawobject->m_VertexCoords.reserve(shapes[s].mesh.indices.size());
			drawobject->m_VertexIndices.reserve(shapes[s].mesh.indices.size());
			drawobject->m_VertexColor.reserve(shapes[s].mesh.indices.size());
			drawobject->m_GeometricNormal.reserve(shapes[s].mesh.indices.size());
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

#ifdef USE_EBO
				// Create vertex indices
				IndexVertex(vertex_index,geometric_normal,vertex0,vertex_num,drawobject.get(),attrib);
				IndexVertex(vertex_index,geometric_normal,vertex1,vertex_num,drawobject.get(),attrib);
				IndexVertex(vertex_index,geometric_normal,vertex2,vertex_num,drawobject.get(),attrib);
				
#else

				// Add vertices
				drawobject->m_VertexCoords.emplace_back(vertex_tmp0);
				drawobject->m_VertexCoords.emplace_back(vertex_tmp1);
				drawobject->m_VertexCoords.emplace_back(vertex_tmp2);

				// Update max and min vertex
				UpdateBoundaryCoords(vertex_tmp0);
				UpdateBoundaryCoords(vertex_tmp1);
				UpdateBoundaryCoords(vertex_tmp2);

				// Add texcoords
				drawobject->m_TexCoords.emplace_back(texcoord0);
				drawobject->m_TexCoords.emplace_back(texcoord1);
				drawobject->m_TexCoords.emplace_back(texcoord2);

				// Add vertex normals
				if (attrib.normals.size())
				{
					drawobject->m_VertexNormals.emplace_back(glm::vec3(normal0));
					drawobject->m_VertexNormals.emplace_back(glm::vec3(normal1));
					drawobject->m_VertexNormals.emplace_back(glm::vec3(normal2));
				}
				else
				{
					drawobject->m_VertexNormals.emplace_back(g_vertex_normal[idx0.vertex_index]);
					drawobject->m_VertexNormals.emplace_back(g_vertex_normal[idx1.vertex_index]);
					drawobject->m_VertexNormals.emplace_back(g_vertex_normal[idx2.vertex_index]);
				}

				// Add geometric normal
				for (int k = 0; k < 3; ++k)
					drawobject->m_GeometricNormal.emplace_back(geometric_normal);
#endif

			} // Loop on indices

			// Compute vertex color per face
			auto face_num = shapes[s].mesh.num_face_vertices.size();

			// Initialize color array
			vertex_num = drawobject->m_VertexCoords.size();
			for (size_t i = 0; i < vertex_num; ++i)
				drawobject->m_VertexColor.emplace_back(glm::vec3(0.0f,0.0f,0.0f));

			// Compute colors
			for (size_t f = 0; f < face_num; ++f)
			{

				auto current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					// Default material is added to the last item in `materials`.
					current_material_id = materials.size() - 1; 
				}

				glm::vec3 diffuse = glm::vec3(materials[current_material_id].diffuse[0],
											materials[current_material_id].diffuse[1],
											materials[current_material_id].diffuse[2]
				);

				glm::vec3 normal0,normal1,normal2;

#ifdef USE_EBO

#ifdef USE_GEOMETRIC_NORMAL

				normal0 = drawobject->m_GeometricNormal[drawobject->m_VertexIndices[3 * f + 0]];
				normal1 = drawobject->m_GeometricNormal[drawobject->m_VertexIndices[3 * f + 1]];
				normal2 = drawobject->m_GeometricNormal[drawobject->m_VertexIndices[3 * f + 2]];

#else

				normal0 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 0]];
				normal1 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 1]];
				normal2 = drawobject->m_VertexNormals[drawobject->m_VertexIndices[3 * f + 2]];

#endif

#else

#ifdef USE_GEOMETRIC_NORMAL

				normal0 = drawobject->m_GeometricNormal[3 * f + 0];
				normal1 = drawobject->m_GeometricNormal[3 * f + 1];
				normal2 = drawobject->m_GeometricNormal[3 * f + 2];

#else

				normal0 = drawobject->m_VertexNormals[3 * f + 0];
				normal1 = drawobject->m_VertexNormals[3 * f + 1];
				normal2 = drawobject->m_VertexNormals[3 * f + 2];

#endif

#endif
				glm::vec3 color0 = normal0 * CXC_NORMAL_FACTOR + diffuse * CXC_DIFFUSE_FACTOR;
				glm::vec3 color1 = normal1 * CXC_NORMAL_FACTOR + diffuse * CXC_DIFFUSE_FACTOR;
				glm::vec3 color2 = normal2 * CXC_NORMAL_FACTOR + diffuse * CXC_DIFFUSE_FACTOR;

				color0 = color0 * glm::vec3(0.5f, 0.5f, 0.5f) + glm::vec3(0.5f, 0.5f, 0.5f);
				color1 = color1 * glm::vec3(0.5f, 0.5f, 0.5f) + glm::vec3(0.5f, 0.5f, 0.5f);
				color2 = color2 * glm::vec3(0.5f, 0.5f, 0.5f) + glm::vec3(0.5f, 0.5f, 0.5f);

#ifdef USE_EBO

				drawobject->m_VertexColor[drawobject->m_VertexIndices[3 * f + 0]] = color0;
				drawobject->m_VertexColor[drawobject->m_VertexIndices[3 * f + 1]] = color1;
				drawobject->m_VertexColor[drawobject->m_VertexIndices[3 * f + 2]] = color2;

#else

				drawobject->m_VertexColor[3 * f + 0] = color0;
				drawobject->m_VertexColor[3 * f + 1] = color1;
				drawobject->m_VertexColor[3 * f + 2] = color2;

#endif

			}
			
			drawobject->Num_of_faces = shapes[s].mesh.num_face_vertices.size();
			drawobject->m_MaterialIDs.reserve(drawobject->Num_of_faces);

			drawobject->ComputeCenterPoint();

			// do not support texturing pre-face
			if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s)
			{
				auto i = shapes[s].mesh.material_ids[s];
				if (i >= 0) 
					m_Material.insert(std::make_pair(shapes[s].name, materials[i]));
				else
					m_Material.insert(std::make_pair(shapes[s].name,materials.back()));
			}
			else
			{
				assert(materials.size() > 0);
				m_Material.insert(std::make_pair(shapes[s].name, materials.back()));
			}

			m_ModelMap.insert(std::make_pair(shapes[s].name,std::move(drawobject)));

		} // shapes
		
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

	void Object3D ::Translation(const std::string &ModelName, const glm::vec3 &TranslationVector) noexcept
	{

		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		
		// Perform translation on root node
		ModelPtr->Translate(TranslationVector);
		
		// Perform translation on children node
		for (auto &it : ObjectNodePtr->children)
			Translation(it->root->GetModelName(),TranslationVector);

		SetStateChanged(GL_TRUE);

	}

	void Object3D ::Rotation(const std::string &ModelName, const glm::vec3 &RotationAxis, float Degree) noexcept
	{
	
		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// Perform rotation on root node
		ModelPtr->Rotate(RotationAxis,Degree);

		// Perform rotation on children node
		for (auto &it : ObjectNodePtr->children)
			Rotation(it->root->GetModelName(),RotationAxis,Degree);
			
		SetStateChanged(GL_TRUE);
		
	}

	void Object3D ::Scaling(const std::string &ModelName, const glm::vec3 &ScalingVector) noexcept
	{
		
		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// Perform the transformation
		ModelPtr->Scale(ScalingVector);

		// Perform the same transforamtion to it's children node;
		for (auto &it : ObjectNodePtr->children)
		{
			auto model = it->root;
			Scaling(model->GetModelName(), ScalingVector);
		}

		SetStateChanged(GL_TRUE);
		
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

	void Object3D::GetObjectBuffers(std::vector<glm::vec3> &vbo_pos, std::vector<VertexAttri> &vbo_attrib, std::vector<uint32_t> &ebo) noexcept
	{
		for (auto shape : m_ModelMap)
		{
			auto vnum = shape.second->GetVertexNum();
			auto inum = shape.second->GetVertexIndices().size();
			auto vertex_coords = shape.second->GetVertexArray();
			std::vector<glm::vec3> normals;
			if (shape.second->GetNormalArray().empty())
				normals = shape.second->GetGeometricNormal();
			else
				normals = shape.second->GetNormalArray();
			auto texcoord = shape.second->GetTexCoordArray();
			auto color = shape.second->GetColorArray();
			auto indices = shape.second->GetVertexIndices();

			auto it = m_VertexSubscript.find(shape.second->GetModelName());
			if (it == m_VertexSubscript.end())
				m_VertexSubscript.insert(std::make_pair(shape.second->GetModelName(), vbo_pos.size()));

			for (size_t k = 0; k<inum; k++)
				ebo.push_back(vbo_pos.size() + indices[k]);

			for (size_t i = 0; i < vnum; ++i) {
				vbo_attrib.emplace_back(VertexAttri(texcoord[i], normals[i], color[i]));
				vbo_pos.emplace_back(vertex_coords[i]);
			}
			
		}
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

	void Object3D::releaseBuffers() noexcept
	{
		if (VAO)
		{
			glDeleteVertexArrays(1, &VAO);
		}

		if (EBO)
		{
			glDeleteBuffers(1, &EBO);
		}

		if (VBO_P)
		{
			glDeleteBuffers(1, &VBO_P);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		}

		if (VBO_A)
		{
			glDeleteBuffers(1, &VBO_A);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
			glDisableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
			glDisableVertexAttribArray(static_cast<GLuint>(Location::COLOR_LOCATION));
		}
	}

	void Object3D::InitBuffers() noexcept
	{
		std::vector<VertexAttri> m_VertexAttribs;
		std::vector<glm::vec3> m_VertexPos;
		std::vector<uint32_t> m_ElementBuffer;

		m_VertexPos.clear();
		m_VertexAttribs.clear();
		m_ElementBuffer.clear();
		uint32_t TotalVexNum = 0U;

		uint32_t vex_num = 0U;

		for (auto shape : m_ModelMap){
			indices_num += shape.second->GetVertexIndices().size();
			vex_num += shape.second->GetVertexArray().size();
		}

		m_VertexPos.reserve(vex_num);
		m_VertexAttribs.reserve(vex_num);
		m_ElementBuffer.reserve(indices_num);

		GetObjectBuffers(m_VertexPos, m_VertexAttribs, m_ElementBuffer);

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

	}

	void Object3D::DrawObject() noexcept
	{
		auto Engine = EngineFacade::GetInstance();

		auto ProgramID = Engine->GetRendermanagerPtr()->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);

		Engine->BindCameraUniforms();
		Engine->GetRendermanagerPtr()->BindLightingUniforms();

		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

		GLuint M_MatrixID = glGetUniformLocation(ProgramID, "M");
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &m_ModelMatrix[0][0]);

		TexSamplerHandle = glGetUniformLocation(ProgramID, "Sampler");
		auto tex_flag_loc = glGetUniformLocation(ProgramID, "tex_is_used");

		// Reset the flag
		glUniform1f(tex_flag_loc, 0.0f);

		glBindTexture(GL_TEXTURE_2D, 0);

		for (auto tex_name : m_TexNames)
		{
			auto tex_ptr = SceneManager::GetInstance()->GetTextureManagerPtr()->GetTexPtr(tex_name);
			if (tex_ptr)
			{

				glBindTexture(GL_TEXTURE_2D, tex_ptr->GetTextureID());

				glUniform1i(TexSamplerHandle,0);

				glUniform1f(tex_flag_loc, 1.0f);

			}
		}
		
		// Updating coordinates position
		g_lock.lock();
		if (CheckStateChanged())
		{
			for (auto shape : m_ModelMap)
			{
				if (shape.second->CheckStateChanged())
				{
					shape.second->UpDateCurrentPos();

					auto vertices = shape.second->GetVertexArray();
					auto offset = GetVertexSubscript(shape.second->GetModelName()) * sizeof(glm::vec3);
					glBindBuffer(GL_ARRAY_BUFFER, VBO_P);
					glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(glm::vec3) * vertices.size(), &vertices.front());

					shape.second->SetStateChanged(GL_FALSE);
				}
			}
			SetStateChanged(GL_FALSE);
		}
		g_lock.unlock();
		
		glBindVertexArray(VAO);

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

		glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, (void*)0);

	}

	void Object3D::RotateWithArbitraryAxis(const std::string &ModelName, const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept
	{

		auto ModelPtr = GetModelByName(ModelName);
		if (!ModelPtr)
			return;

		auto ObjectNodePtr = ModelPtr->m_MyPtr;
		if (!ObjectNodePtr)
			return;

		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied


		// Perform rotation on root node
		// Move to world origin

		Translation(ModelName, -start);

		// Rotation with coordinate axis
		Rotation(ModelName, direction, degree);

		// Move back
		Translation(ModelName, start);

	}
}

