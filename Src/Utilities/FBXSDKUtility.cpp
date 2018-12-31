#include "Utilities/FBXSDKUtility.h"
#include "Core/EngineTypes.h"
#include "Material/MaterialManager.h"
#include "Geometry/SubMesh.h"
#include "Geometry/Mesh.h"
#include "Utilities/DebugLogger.h"

#ifdef IOS_REF
	#undef IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

const int TRIANGLE_VERTEX_COUNT = 3;

// Four floats for every position.
const int VERTEX_STRIDE = 4;
// Three floats for every normal.
const int NORMAL_STRIDE = 3;
// Two floats for every UV.
const int UV_STRIDE = 2;

namespace cxc {

	FBXSDKUtil::FBXSDKUtil()
	{

	}

	FBXSDKUtil::~FBXSDKUtil()
	{


	}

	FbxDouble3 FBXSDKUtil::GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
		const char * pPropertyName,
		const char * pFactorPropertyName,
		GLuint & pTextureName)
	{
		FbxDouble3 lResult(0, 0, 0);
		const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
		const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
		if (lProperty.IsValid() && lFactorProperty.IsValid())
		{
			lResult = lProperty.Get<FbxDouble3>();
			double lFactor = lFactorProperty.Get<FbxDouble>();
			if (lFactor != 1)
			{
				lResult[0] *= lFactor;
				lResult[1] *= lFactor;
				lResult[2] *= lFactor;
			}
		}

		if (lProperty.IsValid())
		{
			const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
			if (lTextureCount)
			{
				const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
				if (lTexture && lTexture->GetUserDataPtr())
				{
					pTextureName = *(static_cast<GLuint *>(lTexture->GetUserDataPtr()));
				}
			}
		}

		return lResult;
	}

	void FBXSDKUtil::GetMaterialProperties(const FbxSurfaceMaterial* pMaterial,
		glm::vec3& Emissive, glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular,
		GLuint& EmissiveTexName, GLuint& AmbientTexName, GLuint& DiffuseTexName, GLuint& SpecularTexName,
		float& Shiniess)
	{
		if (pMaterial)
		{
			// Get the emissive factor of the material
			const FbxDouble3 lEmissive = GetMaterialProperty(pMaterial,
				FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, EmissiveTexName);
			Emissive.x = static_cast<GLfloat>(lEmissive[0]);
			Emissive.y = static_cast<GLfloat>(lEmissive[1]);
			Emissive.z = static_cast<GLfloat>(lEmissive[2]);

			// Get the ambient factor of the material
			const FbxDouble3 lAmbient = GetMaterialProperty(pMaterial,
				FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, AmbientTexName);
			Ambient.x = static_cast<GLfloat>(lAmbient[0]);
			Ambient.y = static_cast<GLfloat>(lAmbient[1]);
			Ambient.z = static_cast<GLfloat>(lAmbient[2]);

			// Get the diffuse factor of the material
			const FbxDouble3 lDiffuse = GetMaterialProperty(pMaterial,
				FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, DiffuseTexName);
			Diffuse.x = static_cast<GLfloat>(lDiffuse[0]);
			Diffuse.y = static_cast<GLfloat>(lDiffuse[1]);
			Diffuse.z = static_cast<GLfloat>(lDiffuse[2]);

			// Get the specular factor of the material
			const FbxDouble3 lSpecular = GetMaterialProperty(pMaterial,
				FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, SpecularTexName);
			Specular.x = static_cast<GLfloat>(lSpecular[0]);
			Specular.y = static_cast<GLfloat>(lSpecular[1]);
			Specular.z = static_cast<GLfloat>(lSpecular[2]);

			// Get the shiness of the material
			FbxProperty lShiniessProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
			if (lShiniessProperty.IsValid())
			{
				Shiniess = static_cast<GLfloat>(lShiniessProperty.Get<FbxDouble>());
			}
		}
	}

	void FBXSDKUtil::GetTexturesFromMaterial(FbxSurfaceMaterial* pSurfaceMaterial, std::vector<std::shared_ptr<Texture2D>>& OutTextures)
	{
		if (pSurfaceMaterial)
		{
			// Go through all the possible textures
			int lTextureIndex;
			FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
			{
				FbxProperty lProperty;
				lProperty = pSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);

				int lTextureCount = lProperty.GetSrcObjectCount<FbxTexture>();
				if (lProperty.IsValid())
				{
					for (int j = 0; j < lTextureCount; ++j)
					{
						// Here we check if it's layered textures, or just textures:
						FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(j);
						if (lLayeredTexture)
						{
							int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
							for (int k = 0; k < lNbTextures; ++k)
							{
								FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
								if (lTexture)
								{
									FbxLayeredTexture::EBlendMode lBlendMode;
									lLayeredTexture->GetTextureBlendMode(k, lBlendMode);

									// To do:
								}
							}
						}
						else
						{
							// No layered texutre simply get on the property
							FbxTexture* lTexture = lProperty.GetSrcObject<FbxTexture>(j);
							if (lTexture)
							{
								// Loading the texture 
								FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
								if (lFileTexture)
								{
									auto pTextureMgr = TextureManager::GetInstance();
									auto pTexture2D = pTextureMgr->LoadTexture(lTexture->GetName(), lFileTexture->GetFileName());
									if (pTexture2D)
										OutTextures.push_back(pTexture2D);
									else
										DEBUG_LOG(eLogType::Error, "FBXSDKUtil::GetTexturesFromMaterial, Can't load the texture : " + (std::string)lFileTexture->GetFileName());
								}
							}
						}
					}
				}
			}
		}
	}

	bool FBXSDKUtil::GetMeshFromNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Mesh>>& OutMeshes, dWorldID WorldID, dSpaceID SpaceID, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<Mesh> pParentNode)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyMesh = false;
		std::shared_ptr<Mesh> pNewMesh = nullptr;
		FbxAMatrix lGlobalOffPosition;

		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh)
		{
			const int lPolygonCount = pMesh->GetPolygonCount();
			bHasFoundAnyMesh = true;

			// Load the material for the object
			auto pMaterialMgr = MaterialManager::GetInstance();
			FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
			FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
			std::vector<std::shared_ptr<SubMesh>> NewSubMeshes;
			if (pMesh->GetElementMaterial())
			{
				lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
				lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
				auto lMaterialReferenceMode = pMesh->GetElementMaterial()->GetReferenceMode();

				if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eAllSame)
				{
					int MaterialIndex;
					if (lMaterialReferenceMode == FbxGeometryElement::eDirect)
					{
						MaterialIndex = 0;
					}
					else if (lMaterialReferenceMode == FbxGeometryElement::eIndex ||
						lMaterialReferenceMode == FbxGeometryElement::eIndexToDirect)
					{
						MaterialIndex = lMaterialIndice->GetAt(0);
					}

					// Get the material
					auto SurfaceMaterial = pNode->GetMaterial(MaterialIndex);
					if (SurfaceMaterial)
					{
						GLuint EmissiveTextureName, AmbientTextureName, DiffuseTextureName, SpecularTextureName;
						glm::vec3 EmissiveFactor, DiffuseFactor, SpecularFactor, AmbientFactor;
						float lShiniess;

						// Get material properties
						GetMaterialProperties(SurfaceMaterial, EmissiveFactor, AmbientFactor, DiffuseFactor, SpecularFactor,
							EmissiveTextureName, AmbientTextureName, DiffuseTextureName, SpecularTextureName, lShiniess);

						// Get textures from material
						std::vector<std::shared_ptr<Texture2D>> OutTextures;
						GetTexturesFromMaterial(SurfaceMaterial, OutTextures);

						std::string MaterialName = SurfaceMaterial->GetName();
						auto LoadedMaterial = NewObject<Material>(MaterialName, EmissiveFactor, AmbientFactor, DiffuseFactor, SpecularFactor, lShiniess);
						auto pSubNewMesh = NewObject<SubMesh>();

						LoadedMaterial->pTextures = std::move(OutTextures);

						pMaterialMgr->addMaterial(LoadedMaterial);
						pSubNewMesh->SetSubMeshMaterial(LoadedMaterial);
						NewSubMeshes.push_back(pSubNewMesh);
					}
				}
				else if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
				{
					FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
					if (lMaterialIndice->GetCount() == lPolygonCount)
					{
						std::unordered_set<uint32_t> MaterialSet;

						// Count the faces of the material
						for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
						{
							const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);

							auto MaterialIter = MaterialSet.find(lMaterialIndex);
							if (MaterialIter != MaterialSet.end())
								continue;

							MaterialSet.insert(lMaterialIndex);

							auto PolygonMaterial = pNode->GetMaterial(lMaterialIndex);
							FBX_ASSERT(PolygonMaterial != nullptr);

							// Get material properties
							GLuint EmissiveTextureName, AmbientTextureName, DiffuseTextureName, SpecularTextureName;
							glm::vec3 EmissiveFactor, DiffuseFactor, SpecularFactor, AmbientFactor;
							float lShiniess;

							// Get material properties
							GetMaterialProperties(PolygonMaterial, EmissiveFactor, AmbientFactor, DiffuseFactor, SpecularFactor,
								EmissiveTextureName, AmbientTextureName, DiffuseTextureName, SpecularTextureName, lShiniess);

							// Get the textures of the material
							std::vector<std::shared_ptr<Texture2D>> OutTextures;
							GetTexturesFromMaterial(PolygonMaterial, OutTextures);

							std::string PolygonMaterialName = PolygonMaterial->GetName();
							auto LoadedPolygonMaterial = NewObject<Material>(PolygonMaterialName, EmissiveFactor, AmbientFactor, DiffuseFactor, SpecularFactor, lShiniess);
							auto pSubNewMesh = NewObject<SubMesh>();

							LoadedPolygonMaterial->pTextures = OutTextures;

							pMaterialMgr->addMaterial(LoadedPolygonMaterial);
							pSubNewMesh->SetSubMeshMaterial(LoadedPolygonMaterial);
							NewSubMeshes.push_back(pSubNewMesh);
						}
					}
				}
			} // if Meterial
			else
			{
				// When no materials assigned, create the mesh with default material
				auto pSubMeshWithDefaultMaterial = std::make_shared<SubMesh>();
				auto pDefaultMaterial = pMaterialMgr->GetMaterial("DefaultMaterial");
				FBX_ASSERT(pDefaultMaterial != nullptr);

				pSubMeshWithDefaultMaterial->SetSubMeshMaterial(pDefaultMaterial);
				NewSubMeshes.push_back(pSubMeshWithDefaultMaterial);
			}

			// Congregate all the data of a mesh to be cached in VBOs
			// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex
			bool bHasNormal = pMesh->GetElementNormalCount() > 0;
			bool bHasUV = pMesh->GetElementUVCount() > 0;
			bool bAllByControlPoint = true;
			FbxGeometryElement::EMappingMode NormalMappingMode = FbxGeometryElement::eNone;
			FbxGeometryElement::EMappingMode UVMappingMode = FbxGeometryElement::eNone;
			if (bHasNormal)
			{
				NormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
				if (NormalMappingMode == FbxGeometryElement::eNone)
				{
					bHasNormal = false;
				}
				if (bHasNormal && NormalMappingMode != FbxGeometryElement::eByControlPoint)
				{
					bAllByControlPoint = false;
				}
			}

			// Allocate the array memory, by control point or by polygon vertex
			int PolygonVertexCount = pMesh->GetControlPointsCount();
			if (!bAllByControlPoint)
			{
				PolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
			}

			std::vector<glm::vec3> Vertices;
			std::vector<glm::vec3> Normals;
			std::vector<glm::vec2> UVs;
			std::vector<uint32_t> Indices;
			std::map<VertexIndexPacket, size_t> VertexIndexingMap;

			FbxStringList UVNames;
			pMesh->GetUVSetNames(UVNames);
			const char* UVName = nullptr;
			if (bHasUV && UVNames.GetCount())
			{
				UVName = UVNames[0];
			}

			// Populate the array with vertex attribute, if by control point
			const FbxVector4* ControlPoints = pMesh->GetControlPoints();
			FbxVector4 CurrentVertex;
			FbxVector4 CurrentNormal;
			FbxVector2 CurrentUV;
			if (bAllByControlPoint)
			{
				const FbxGeometryElementNormal* NormalElement = nullptr;
				const FbxGeometryElementUV* UVElement = nullptr;
				if (bHasNormal)
				{
					NormalElement = pMesh->GetElementNormal(0);
				}
				if (bHasUV)
				{
					UVElement = pMesh->GetElementUV(0);
				}
				for (int Index = 0; Index < PolygonVertexCount; ++Index)
				{
					// Save the vertex position
					CurrentVertex = ControlPoints[Index];
					Vertices.push_back(glm::vec3(
						static_cast<float>(CurrentVertex[0]),
						static_cast<float>(CurrentVertex[1]),
						static_cast<float>(CurrentVertex[2])
					));

					// Save the normal
					if (bHasNormal)
					{
						int NormalIndex = Index;
						if (NormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							NormalIndex = NormalElement->GetIndexArray().GetAt(Index);
						}

						CurrentNormal = NormalElement->GetDirectArray().GetAt(NormalIndex);
						Normals.push_back(glm::vec3(
							static_cast<float>(CurrentNormal[0]),
							static_cast<float>(CurrentNormal[1]),
							static_cast<float>(CurrentNormal[2])
						));
					}

					// Save the UV.
					if (bHasUV)
					{
						int UVIndex = Index;
						if (UVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							UVIndex = UVElement->GetIndexArray().GetAt(Index);
						}

						CurrentUV = UVElement->GetDirectArray().GetAt(UVIndex);
						UVs.push_back(glm::vec2(
							static_cast<float>(CurrentUV[0]),
							1.0f - static_cast<float>(CurrentUV[1])
						));
					}
				}
			}

			for (int PolygonIndex = 0; PolygonIndex < lPolygonCount; ++PolygonIndex)
			{
				uint32_t MeshVertexIndex[3];

				for (int lVerticesIndex = 0; lVerticesIndex < TRIANGLE_VERTEX_COUNT; ++lVerticesIndex)
				{
					const int lControlPointIndex = pMesh->GetPolygonVertex(PolygonIndex, lVerticesIndex);

					if (bAllByControlPoint)
					{
						Indices.push_back(static_cast<uint32_t>(lControlPointIndex));
					}
					else
					{
						// Populate the array with vertex attribute, if by polygon vertex
						glm::vec3 VertexPos, VertexNormal;
						glm::vec2 VertexUV;

						CurrentVertex = ControlPoints[lControlPointIndex];
						VertexPos = glm::vec3(
							static_cast<float>(CurrentVertex[0]),
							static_cast<float>(CurrentVertex[1]),
							static_cast<float>(CurrentVertex[2])
						);

						if (bHasNormal)
						{
							pMesh->GetPolygonVertexNormal(PolygonIndex, lVerticesIndex, CurrentNormal);
							VertexNormal = glm::vec3(
								static_cast<float>(CurrentNormal[0]),
								static_cast<float>(CurrentNormal[1]),
								static_cast<float>(CurrentNormal[2])
							);
						}

						if (bHasUV)
						{
							bool bUnmappedUV;
							pMesh->GetPolygonVertexUV(PolygonIndex, lVerticesIndex, UVName, CurrentUV, bUnmappedUV);
							VertexUV = glm::vec2(
								static_cast<float>(CurrentUV[0]),
								1.0f - static_cast<float>(CurrentUV[1])
							);
						}

						// Vertex indexing
						VertexIndexPacket VertexPacket(VertexPos, VertexNormal, VertexUV);
						auto iter = VertexIndexingMap.find(VertexPacket);
						if (iter != VertexIndexingMap.end())
						{
							Indices.push_back(iter->second);
							MeshVertexIndex[lVerticesIndex] = iter->second;
						}
						else
						{
							Vertices.emplace_back(VertexPos);
							Normals.emplace_back(VertexNormal);
							UVs.emplace_back(VertexUV);
							Indices.push_back(Vertices.size() - 1);

							VertexIndexingMap.insert(std::make_pair(VertexPacket, Vertices.size() - 1));

							MeshVertexIndex[lVerticesIndex] = Vertices.size() - 1;
						}
					}
				}

				if (lMaterialIndice && lMaterialIndice->GetCount() > PolygonIndex)
				{
					auto CurrentMaterialIndex = lMaterialIndice->GetAt(PolygonIndex);
					auto CurrentMaterial = pNode->GetMaterial(CurrentMaterialIndex);
					FBX_ASSERT(CurrentMaterial != nullptr);

					for (auto pSubMesh : NewSubMeshes)
					{
						if (pSubMesh->pMaterial->MaterialName == CurrentMaterial->GetName())
						{
							pSubMesh->Indices.push_back(MeshVertexIndex[0]);
							pSubMesh->Indices.push_back(MeshVertexIndex[1]);
							pSubMesh->Indices.push_back(MeshVertexIndex[2]);
						}
					}
				}
			}

			// Create the object
			pNewMesh = std::make_shared<Mesh>(Vertices, Normals, UVs, Indices);
			pNewMesh->MeshName = pNode->GetName();
			pNewMesh->isLoaded = true;

			// Create indice of the meshes
			if (!pMesh->GetElementMaterial() || lMaterialMappingMode == FbxGeometryElement::eAllSame)
			{
				FBX_ASSERT(NewSubMeshes.size() == 1);
				NewSubMeshes[0]->Indices = pNewMesh->m_VertexIndices;
				NewSubMeshes[0]->SetOwnerObject(pNewMesh);

				pNewMesh->SubMeshes = NewSubMeshes;
			}
			else if (lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				FBX_ASSERT(NewSubMeshes.size() == lPolygonCount);

				for (auto mesh : NewSubMeshes)
				{
					mesh->SetOwnerObject(pNewMesh);
				}

				pNewMesh->SubMeshes = NewSubMeshes;
			}

			// Create the parent-child linkage
			if (pParentNode)
			{
				pParentNode->pChildNodes.push_back(pNewMesh);
				pNewMesh->pParentNode = pParentNode;
			}

			// Get global position of the node
			FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);
			FbxAMatrix lGeometryOffset = GetGeometry(pNode);
			lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

			// Set translation
			pNewMesh->Translate(glm::vec3(lGlobalPosition.GetT()[0], lGlobalPosition.GetT()[1], lGlobalPosition.GetT()[2]));

			// Set rotation
			pNewMesh->RotateLocalSpace(glm::vec3(1, 0, 0), static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[0])));
			pNewMesh->RotateLocalSpace(glm::vec3(0, 1, 0), static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[1])));
			pNewMesh->RotateLocalSpace(glm::vec3(0, 0, 1), static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[2])));

			// Set sacling
			pNewMesh->Scale(glm::vec3(glm::vec3(lGlobalPosition.GetS()[0], lGlobalPosition.GetS()[1], lGlobalPosition.GetS()[2])));

			OutMeshes.push_back(pNewMesh);
		}

		// Recursively traverse each node in the scene
		int i, lCount = pNode->GetChildCount();
		for (i = 0; i < lCount; i++)
		{
			bHasFoundAnyMesh |= GetMeshFromNode(pNode->GetChild(i), OutMeshes, WorldID, SpaceID, lGlobalOffPosition, pNewMesh);
		}

		return bHasFoundAnyMesh;
	}

	FbxAMatrix FBXSDKUtil::GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
	{
		FbxAMatrix lGlobalPosition;
		bool        lPositionFound = false;

		if (pPose)
		{
			int lNodeIndex = pPose->Find(pNode);

			if (lNodeIndex > -1)
			{
				// The bind pose is always a global matrix.
				// If we have a rest pose, we need to check if it is
				// stored in global or local space.
				if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
				{
					lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
				}
				else
				{
					// We have a local matrix, we need to convert it to
					// a global space matrix.
					FbxAMatrix lParentGlobalPosition;

					if (pParentGlobalPosition)
					{
						lParentGlobalPosition = *pParentGlobalPosition;
					}
					else
					{
						if (pNode->GetParent())
						{
							lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
						}
					}

					FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
					lGlobalPosition = lParentGlobalPosition * lLocalPosition;
				}

				lPositionFound = true;
			}
		}

		if (!lPositionFound)
		{
			// There is no pose entry for that node, get the current global position instead.

			// Ideally this would use parent global position and local position to compute the global position.
			// Unfortunately the equation 
			//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
			// does not hold when inheritance type is other than "Parent" (RSrs).
			// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
			lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
		}

		return lGlobalPosition;
	}

	// Get the matrix of the given pose
	FbxAMatrix FBXSDKUtil::GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
	{
		FbxAMatrix lPoseMatrix;
		FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

		memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

		return lPoseMatrix;
	}

	// Get the geometry offset to a node. It is never inherited by the children.
	FbxAMatrix FBXSDKUtil::GetGeometry(FbxNode* pNode)
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxAMatrix(lT, lR, lS);
	}

	bool FBXSDKUtil::GetLightFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<LightSource>>& OutLights, FbxAMatrix& pParentGlobalPosition)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyLightNode = false;
		std::shared_ptr<LightSource> pNewLight = std::make_shared<LightSource>();

		// Get light from root node
		FbxLight* lLight = pNode->GetLight();
		if (lLight)
		{
			// Marked as successful
			bHasFoundAnyLightNode = true;

			// Light Name
			pNewLight->LightName = pNode->GetName();

			// Light decay type
			auto DecayType = lLight->DecayType.Get();
			switch (DecayType)
			{
			case fbxsdk::FbxLight::eNone:
				pNewLight->AtteunationType = eLightAtteunationType::None;
				break;
			case fbxsdk::FbxLight::eLinear:
				pNewLight->AtteunationType = eLightAtteunationType::Linear;
				break;
			case fbxsdk::FbxLight::eQuadratic:
				pNewLight->AtteunationType = eLightAtteunationType::Quadratic;
				break;
			case fbxsdk::FbxLight::eCubic:
				pNewLight->AtteunationType = eLightAtteunationType::Cubic;
				break;
			}

			// Light color
			FbxDouble3 LightColor = lLight->Color.Get();
			pNewLight->LightColor = glm::vec3(LightColor[0], LightColor[1], LightColor[2]);

			// Light intensity
			pNewLight->LightIntensity = lLight->Intensity.Get();

			// Whether to cast light
			pNewLight->bCastLight = lLight->CastLight.Get();

			// Whether to cast shadow
			pNewLight->bCastShadow = lLight->CastShadows.Get();
		
			FbxAMatrix TargetGlobalOffPosition;
			// Light type
			switch (lLight->LightType.Get())
			{
			case 0 :
				// Omnidirectional light
				pNewLight->LightType = eLightType::OmniDirectional;
				pNewLight->AtteunationType = eLightAtteunationType::Quadratic;
				pNewLight->SetIntensity(1.0f);
				break;
			case 1:
			{
				// Directional light
				pNewLight->LightType = eLightType::Directional;
				break;
			}
			case 2:
			{
				// Spot light
				pNewLight->LightType = eLightType::Spot;
				pNewLight->CutOffAngle = lLight->OuterAngle.Get();
				break;
			}
			case 3:
				// Area light
				pNewLight->LightType = eLightType::Area;
				break;
			case 4:
				// Volumetric light
				pNewLight->LightType = eLightType::Volumetric;
				break;
			default:
				// Invalid light type
				pNewLight->LightType = eLightType::InvalidType;
				break;
			}

			// Get the target of the directional light
			auto pTargetNode = pNode->GetTarget();
			if (pTargetNode)
			{
				TargetGlobalOffPosition = GetGlobalPosition(pTargetNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);
				FbxAMatrix lTargetNodeGeometryOfferset = GetGeometry(pTargetNode);
				FbxAMatrix lTargetNodeGlobalOffPosition = TargetGlobalOffPosition * lTargetNodeGeometryOfferset;
				pNewLight->SetTargetPos(lTargetNodeGlobalOffPosition.GetT()[0], lTargetNodeGlobalOffPosition.GetT()[1], lTargetNodeGlobalOffPosition.GetT()[2]);
			}

			// Get global position and orientation
			FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);
			FbxAMatrix lGeometryOffset = GetGeometry(pNode);
			FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

			// Set rotation
			auto RotMatrix = glm::rotate(glm::mat4(1.0f), static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[0])), glm::vec3(1, 0, 0));
			RotMatrix = glm::rotate(RotMatrix, static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[1])), glm::vec3(0, 1, 0));
			RotMatrix = glm::rotate(RotMatrix, static_cast<float>(glm::radians(lGlobalOffPosition.GetR()[2])), glm::vec3(0, 0, 1));

			pNewLight->SetLightPos(lGlobalOffPosition.GetT()[0], lGlobalOffPosition.GetT()[1], lGlobalOffPosition.GetT()[2]);

			OutLights.emplace_back(pNewLight);
		}

		// Recursively traverse each node in the scene
		int i, lCount = pNode->GetChildCount();
		for (i = 0; i < lCount; i++)
		{
			bHasFoundAnyLightNode |= GetLightFromRootNode(pNode->GetChild(i), OutLights, pParentGlobalPosition);
		}

		return bHasFoundAnyLightNode;
	}

	bool FBXSDKUtil::GetCameraFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Camera>>& OutCameras)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyCameraNode = false;

		std::shared_ptr<Camera> RetCamera = std::make_shared<Camera>();

		// Get camera from root node
		FbxCamera* lCamera = pNode->GetCamera();
		if (lCamera)
		{
			/* Save the camera configuration */
			// Camera Name
			RetCamera->CameraName = lCamera->GetName();

			// Interest position
			FbxVector4 InterestPosition = lCamera->InterestPosition.Get();
			RetCamera->CameraOrigin = glm::vec3(InterestPosition[0], InterestPosition[1], InterestPosition[2]);

			// Up vector
			FbxVector4 UpVector = lCamera->UpVector.Get();
			RetCamera->UpVector = glm::vec3(UpVector[0], UpVector[1], UpVector[2]);

			// Origin position
			FbxVector4 Origin = lCamera->Position.Get();
			RetCamera->EyePosition = glm::vec3(Origin[0], Origin[1], Origin[2]);

			if (lCamera->ProjectionType.Get() == FbxCamera::eOrthogonal)
			{
				// todo
			}
			else if (lCamera->ProjectionType.Get() == FbxCamera::ePerspective)
			{
				// todo
			}

			OutCameras.emplace_back(RetCamera);
		}
		else
		{
			bHasFoundAnyCameraNode = false;
		}

		// Recursively traverse each node in the scene
		int i, lCount = pNode->GetChildCount();
		for (i = 0; i < lCount; i++)
		{
			bHasFoundAnyCameraNode |= GetCameraFromRootNode(pNode->GetChild(i), OutCameras);
		}

		return bHasFoundAnyCameraNode;
	}

	//This function computes the pixel ratio
	double FBXSDKUtil::ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio)
	{
		if (pWidth < 0.0 || pHeight < 0.0)
			return 0.0;

		pWidth = GET_MAX(pWidth, 1.0);
		pHeight = GET_MAX(pHeight, 1.0);

		double lResolutionRatio = (double)pWidth / pHeight;

		return pScreenRatio / lResolutionRatio;
	}

	void FBXSDKUtil::InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene)
	{
		// Check if the FbxManager has already been created
		if (pManager) return;

		// The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
		pManager = FbxManager::Create();
		if (!pManager)
		{
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX Manager");
			return;
		}
		else
		{
			DEBUG_LOG(eLogType::Verbose, "Autodesk FBX SDK version");
		}

		// Create an IOSettings object. This object holds all import/export settings
		FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(ios);

		// Loading plugins from the executable directory (optional)
		FbxString lpath = FbxGetApplicationDirectory();
		pManager->LoadPluginsDirectory(lpath.Buffer());

		// Create an FBX scene. This object holds most objects imported/exported from/to files.
		pScene = FbxScene::Create(pManager, "MyScene");
		if (!pScene)
		{
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX scene");
			return;
		}
	}

	void FBXSDKUtil::DestroySDKObjects(FbxManager* pManager, bool pExitStatus)
	{
		// Delete the FBX Manager. All the objects that have been allocated using the FBx Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) DEBUG_LOG(eLogType::Verbose, "Autodesk FBX SDK has been run successfully");
	}

	bool FBXSDKUtil::SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat, bool pEmbedMedia)
	{
		int lMajor, lMinor, lRevision;
		bool lStatus = true;

		// Create an exporter
		FbxExporter* lExporter = FbxExporter::Create(pManager, "MyExporter");

		if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount())
		{
			// Write in fall back format in less no ASCII format found
			pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

			// Try to export in ASCII if possiable
			int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

			for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
			{
				if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
				{
					FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
					const char *lASCII = "ascii";
					if (lDesc.Find(lASCII) >= 0)
					{
						pFileFormat = lFormatIndex;
						break;
					}
				}
			}
		}

		// Set the export states. By default, the export states are always set to
		// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below
		// shows how to change these states.
		IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
		IOS_REF.SetBoolProp(EXP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(EXP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

		// Initialize the exporter by providing a filename
		if (lExporter->Initialize(pFileName, pFileFormat, pManager->GetIOSettings()) == false)
		{
			std::string ErrorMsg = "FBXSDKHelper::SaveScene, failed to initialize the FbxExporter, with the errors returned : ";
			ErrorMsg += lExporter->GetStatus().GetErrorString();
			DEBUG_LOG(eLogType::Error, ErrorMsg);
			return false;
		}

		FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
		std::string VersionString = "The FBX file format version is " + 
			std::to_string(lMajor) + "." + std::to_string(lMinor) + "." + std::to_string(lRevision);
		DEBUG_LOG(eLogType::Verbose, VersionString);

		// Export the scene
		lStatus = lExporter->Export(pScene);

		// Destroy the exporter
		lExporter->Destroy();

		return lStatus;
	}

	bool FBXSDKUtil::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		int i, lAnimStackCount;
		bool lStatus;

		// Get the file version number generated by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(pManager, "MyImporter");

		// Initialize the importer by providing a filename
		const bool lImportStatus = lImporter->Initialize(pFileName, -1, pManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if (!lImportStatus)
		{
			std::string ErrorString = "FBXSDKHelper::LoadScene, failed to initializer the FbxImporter, with the errors returned ";
			ErrorString += lImporter->GetStatus().GetErrorString();
			DEBUG_LOG(eLogType::Error, ErrorString);

			if(lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				ErrorString = "FBXSDKHelper::LoadScene, FBX file format version for this FBX SDK is ";
				ErrorString += std::to_string(lSDKMajor) + "." + std::to_string(lSDKMinor) + "." + std::to_string(lSDKRevision);
				DEBUG_LOG(eLogType::Error, ErrorString);
				
				ErrorString = "FBXSDKHelper::LoadScene, While the FBX file format version for file: ";
				ErrorString += pFileName;
				ErrorString += " is " + std::to_string(lFileMajor) + "." + std::to_string(lFileMinor) + "." + std::to_string(lFileRevision);
				DEBUG_LOG(eLogType::Error, ErrorString);
			}

			return false;
		}

		std::string VerboseString = "FBXSDKHelper::LoadScene, FBX file format version for this FBX SDK is ";
		VerboseString += std::to_string(lSDKMajor) + "." + std::to_string(lSDKMinor) + "." + std::to_string(lSDKRevision);
		DEBUG_LOG(eLogType::Verbose, VerboseString);

		if (lImporter->IsFBX())
		{
			VerboseString = "FBXSDKHelper::LoadScene, FBX file format version for file: ";
			VerboseString += std::string(pFileName) + " is " + 
				std::to_string(lFileMajor) + "." + std::to_string(lFileMinor) + "." + std::to_string(lFileRevision);
			DEBUG_LOG(eLogType::Verbose, VerboseString);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.
			
			DEBUG_LOG(eLogType::Verbose, "Animation Stack Information");

			lAnimStackCount = lImporter->GetAnimStackCount();

			DEBUG_LOG(eLogType::Verbose, "Number of Animation Stacks: " + std::to_string(lAnimStackCount));
			DEBUG_LOG(eLogType::Verbose, "Current Animation Stack: " + (std::string)lImporter->GetActiveAnimStackName());

			for (i = 0; i < lAnimStackCount; ++i)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				DEBUG_LOG(eLogType::Verbose, "Animation Stack:" + std::to_string(i));
				DEBUG_LOG(eLogType::Verbose, "Name : " + (std::string)lTakeInfo->mName.Buffer());
				DEBUG_LOG(eLogType::Verbose, "Animation Stack:" + (std::string)lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				DEBUG_LOG(eLogType::Verbose, "Import Name : " + (std::string)lTakeInfo->mImportName.Buffer());
				DEBUG_LOG(eLogType::Verbose, "Import State : " + (std::string)(lTakeInfo->mSelect ? "true" : "false"));
			}

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
			IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
			IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
			IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
			IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
			IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
			IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::SaveScene, Failed to import the scene since it is password protected!");
		}

		// Destroy the importer
		lImporter->Destroy();

		return lStatus;
	}
}