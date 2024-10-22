#include "Core/EngineTypes.h"
#include "Material/MaterialManager.h"
#include "Geometry/SubMesh.h"
#include "Geometry/Mesh.h"
#include "Utilities/DebugLogger.h"
#include "Utilities/FBXSDKUtility.h"
#include "World/World.h"
#include "Scene/SceneContext.h"
#include "Animation/AnimStack.h"
#include "Animation/AnimLayer.h"
#include "Animation/AnimCurve.h"
#include "Animation/AnimKeyFrame.h"
#include "Animation/Pose.h"
#include "Animation/Skeleton.h"
#include "Animation/LinkBone.h"
#include "Animation/Cluster.h"

#ifdef IOS_REF
	#undef IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

// Polygon stride for the triangle
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
									{
										DEBUG_LOG(eLogType::Error, "FBXSDKUtil::GetTexturesFromMaterial, Can't load the texture : ");
										DEBUG_LOG(eLogType::Error, (std::string)lFileTexture->GetFileName() + '\n');
									}
								}
							}
						}
					}
				}
			}
		}
	}

	bool FBXSDKUtil::GetMeshFromNode(FbxNode* pNode, std::shared_ptr<SceneContext> OutSceneContext, dWorldID WorldID, dSpaceID SpaceID, FbxAMatrix& lGlobalPosition, std::shared_ptr<Mesh> pParentNode)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyMesh = false;
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
			auto OutMesh = std::make_shared<Mesh>(Vertices, Normals, UVs, Indices);
			OutMesh->MeshName = pNode->GetName();
			OutMesh->isLoaded = true;

			// Create indice of the meshes
			if (!pMesh->GetElementMaterial() || lMaterialMappingMode == FbxGeometryElement::eAllSame)
			{
				FBX_ASSERT(NewSubMeshes.size() == 1);
				NewSubMeshes[0]->Indices = OutMesh->m_VertexIndices;
				NewSubMeshes[0]->SetOwnerObject(OutMesh);

				OutMesh->SubMeshes = NewSubMeshes;
			}
			else if (lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				FBX_ASSERT(NewSubMeshes.size() == lPolygonCount);

				for (auto mesh : NewSubMeshes)
				{
					mesh->SetOwnerObject(OutMesh);
				}

				OutMesh->SubMeshes = NewSubMeshes;

				// Create the parent-child linkage
				if (pParentNode)
				{
					pParentNode->pChildNodes.push_back(OutMesh);
					OutMesh->pParentNode = pParentNode;
				}
			}

			// Set translation
			OutMesh->Translate(glm::vec3(lGlobalPosition.GetT()[0], lGlobalPosition.GetT()[1], lGlobalPosition.GetT()[2]));

			// Set rotation
			OutMesh->RotateLocalSpace(glm::vec3(1, 0, 0), static_cast<float>(glm::radians(lGlobalPosition.GetR()[0])));
			OutMesh->RotateLocalSpace(glm::vec3(0, 1, 0), static_cast<float>(glm::radians(lGlobalPosition.GetR()[1])));
			OutMesh->RotateLocalSpace(glm::vec3(0, 0, 1), static_cast<float>(glm::radians(lGlobalPosition.GetR()[2])));

			// Set sacling
			OutMesh->Scale(glm::vec3(glm::vec3(lGlobalPosition.GetS()[0], lGlobalPosition.GetS()[1], lGlobalPosition.GetS()[2])));

			OutSceneContext->Meshes.push_back(OutMesh);
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

	void FBXSDKUtil::LoadAnimationStacks(FbxScene* pScene, std::shared_ptr<SceneContext> OutSceneContext)
	{
		int StackCount = pScene->GetSrcObjectCount<FbxAnimStack>();
		for (int i = 0; i < StackCount; ++i)
		{
			FbxAnimStack* FbxAnimationStack = pScene->GetSrcObject<FbxAnimStack>(i);
			auto ExtractedAnimStack = NewObject<AnimStack>(FbxAnimationStack->GetName());

			// Extract animation layers
			ExtractAnimStack(FbxAnimationStack, pScene->GetRootNode(), ExtractedAnimStack);

			OutSceneContext->AnimationStacks.push_back(ExtractedAnimStack);
		}
	}

	int FBXSDKUtil::InterpolationFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
		if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
		if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
		return 0;
	}

	int FBXSDKUtil::ConstantmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
		if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
		return 0;
	}

	int FBXSDKUtil::TangentmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
		if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
		if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
		if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
		if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
		if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
		return 0;
	}

	int FBXSDKUtil::TangentweightFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
		if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
		if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
		return 0;
	}

	int FBXSDKUtil::TangentVelocityFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
		if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
		if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
		return 0;
	}

	std::shared_ptr<AnimCurve> FBXSDKUtil::ExtractListAnimKeyFrames(FbxNode* pNode, FbxAnimCurve* pCurve, FbxProperty* pProperty)
	{
		if (pCurve == nullptr)
			return nullptr;

		auto OutCurve = NewObject<AnimCurve>();

		float KeyTime;
		float KeyValue;
		int KeyCount = pCurve->KeyGetCount();
		for (int i = 0; i < KeyCount; ++i)
		{
			KeyValue = static_cast<float>(pCurve->KeyGetValue(i));;
			KeyTime = pCurve->KeyGetTime(i).GetSecondDouble();

			std::string PropertyEnumValue = pProperty->GetEnumValue(KeyValue);

			auto ExtractedKeyFrame = NewObject<AnimKeyFrame>(KeyTime, KeyValue);
			OutCurve->AddKeyFrame(ExtractedKeyFrame);
		}

		return OutCurve;
	}

	std::shared_ptr<AnimCurve> FBXSDKUtil::ExtractAnimKeyFrames(FbxNode* pNode, FbxAnimCurve* pCurve)
	{
		if (pCurve == nullptr)
			return nullptr;
		
		auto OutCurve = NewObject<AnimCurve>();

		float KeyTime;
		float KeyValue;
		int KeyCount = pCurve->KeyGetCount();
		for (int i = 0; i < KeyCount; ++i)
		{
			KeyValue = static_cast<float>(pCurve->KeyGetValue(i));
			KeyTime = pCurve->KeyGetTime(i).GetSecondDouble();

			// Get the interpolation type
			auto InterpolationType = InterpolationFlagToIndex(pCurve->KeyGetInterpolation(i));

			if ((pCurve->KeyGetInterpolation(i)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
			{
				// Constant mode
				auto ConstantMode = ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(i));
			}
			else if ((pCurve->KeyGetInterpolation(i)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
			{
				// Cubic mode
				auto CubicMode = TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(i));

				// Tangent weight mode
				auto TangentWeightMode = TangentweightFlagToIndex(pCurve->KeyGet(i).GetTangentWeightMode());

				// tangent velocity mode
				auto TangentVelocityMode = TangentVelocityFlagToIndex(pCurve->KeyGet(i).GetTangentVelocityMode());
			}

			auto ExtractedKeyFrame = NewObject<AnimKeyFrame>(KeyTime, KeyValue);
			OutCurve->AddKeyFrame(ExtractedKeyFrame);
		}

		OutCurve->SortKeyFramesByStartTime();

		return OutCurve;
	}

	void FBXSDKUtil::ExtractAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode, std::shared_ptr<AnimLayer> OutAnimLayer, bool bIsSwitcher)
	{
		FbxAnimCurve* lAnimCurve = nullptr;

		// General curves
		if (!bIsSwitcher)
		{
			// Translation of the x axis
			lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Translation of the y axis
			lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Translation of the z axis
			lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Rotation of the x axis
			lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Rotation of the y axis
			lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Rotation of the z axis
			lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Scaling of the x axis
			lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Scaling of the y axis
			lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Scaling of the z axis
			lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}
		}

		// Curve specific to a light or marker
		FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
		if (lNodeAttribute)
		{
			// RGB curve
			lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
			if (lAnimCurve)
			{
				OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
			}

			// Curve specific to a light
			FbxLight* pLight = pNode->GetLight();
			if (pLight)
			{
				// Light attributes
				lAnimCurve = pLight->Intensity.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pLight->Fog.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pLight->OuterAngle.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}
			}

			// Curve specific to a camera
			FbxCamera* pCamera = pNode->GetCamera();
			if (pCamera)
			{
				// Camera attribute
				lAnimCurve = pCamera->FieldOfView.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pCamera->FieldOfViewX.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pCamera->FieldOfViewY.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pCamera->OpticalCenterX.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pCamera->OpticalCenterY.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}

				lAnimCurve = pCamera->Roll.GetCurve(pAnimLayer);
				if (lAnimCurve)
				{
					OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
				}
			}

			// Curves specific to a geometry
			if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
			{
				FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;

				int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
				for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
				{
					FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

					int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
					for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
					{
						FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
						const char* lChannelName = lChannel->GetName();

						lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
						if (lAnimCurve)
							OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
					}
				}
			}

			// Curves specific to properties
			FbxProperty lProperty = pNode->GetFirstProperty();
			while (lProperty.IsValid())
			{
				if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
				{
					FbxString lFbxFCurveNodeName = lProperty.GetName();
					FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);

					if (!lCurveNode) {
						lProperty = pNode->GetNextProperty(lProperty);
						continue;
					}

					FbxDataType lDataType = lProperty.GetPropertyDataType();
					if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
					{
						for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
						{
							lAnimCurve = lCurveNode->GetCurve(0U, c);
							if (lAnimCurve)
								OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
						}
					}
					else if (lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
					{
						for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
						{
							lAnimCurve = lCurveNode->GetCurve(0U, c);
							if (lAnimCurve)
							{
								OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
							}
						}

						for (int c = 0; c < lCurveNode->GetCurveCount(1U); c++)
						{
							lAnimCurve = lCurveNode->GetCurve(1U, c);
							if (lAnimCurve)
							{
								OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
							}
						}

						for (int c = 0; c < lCurveNode->GetCurveCount(2U); c++)
						{
							lAnimCurve = lCurveNode->GetCurve(2U, c);
							if (lAnimCurve)
							{
								OutAnimLayer->AddAnimationCurve(ExtractAnimKeyFrames(pNode, lAnimCurve));
							}
						}
					}
					else if (lDataType.GetType() == eFbxEnum)
					{
						for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
						{
							lAnimCurve = lCurveNode->GetCurve(0U, c);
							if (lAnimCurve)
								OutAnimLayer->AddAnimationCurve(ExtractListAnimKeyFrames(pNode, lAnimCurve, &lProperty));
						}
					}
				}

				lProperty = pNode->GetNextProperty(lProperty);
			}
		}

		// Extract animation curves from child node
		auto ChildNodeCount = pNode->GetChildCount();
		for (auto i = 0; i < ChildNodeCount; ++i)
		{
			ExtractAnimLayer(pAnimLayer, pNode->GetChild(i), OutAnimLayer, bIsSwitcher);
		}
	}

	void FBXSDKUtil::ExtractAnimStack(FbxAnimStack* pAnimStack, FbxNode* pNode, std::shared_ptr<AnimStack> OutAnimStack)
	{
		auto AnimationLayerCount = pAnimStack->GetMemberCount<FbxAnimLayer>();
		for (auto i = 0; i < AnimationLayerCount; ++i)
		{
			FbxAnimLayer* FbxAnimationLayer = pAnimStack->GetMember<FbxAnimLayer>(i);
			auto ExtractedAnimLayer = NewObject<AnimLayer>(FbxAnimationLayer->GetName());

			// Extract animation curves of the layer
			ExtractAnimLayer(FbxAnimationLayer, pNode, ExtractedAnimLayer);

			if (ExtractedAnimLayer->GetAnimCurveCount() > 0)
				OutAnimStack->AddAnimationLayer(ExtractedAnimLayer);
		}
	}

	bool FBXSDKUtil::GetLightFromRootNode(FbxNode* pNode, FbxAMatrix& lGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyLightNode = false;
		auto OutLight = std::make_shared<LightSource>();

		// Get light from root node
		FbxLight* lLight = pNode->GetLight();
		if (lLight)
		{
			// Marked as successful
			bHasFoundAnyLightNode = true;

			// Light Name
			OutLight->LightName = pNode->GetName();

			// Light decay type
			auto DecayType = lLight->DecayType.Get();
			switch (DecayType)
			{
			case fbxsdk::FbxLight::eNone:
				OutLight->AtteunationType = eLightAtteunationType::None;
				break;
			case fbxsdk::FbxLight::eLinear:
				OutLight->AtteunationType = eLightAtteunationType::Linear;
				break;
			case fbxsdk::FbxLight::eQuadratic:
				OutLight->AtteunationType = eLightAtteunationType::Quadratic;
				break;
			case fbxsdk::FbxLight::eCubic:
				OutLight->AtteunationType = eLightAtteunationType::Cubic;
				break;
			}

			// Light color
			FbxDouble3 LightColor = lLight->Color.Get();
			OutLight->LightColor = glm::vec3(LightColor[0], LightColor[1], LightColor[2]);

			// Light intensity
			OutLight->LightIntensity = lLight->Intensity.Get();

			// Whether to cast light
			OutLight->bCastLight = lLight->CastLight.Get();

			// Whether to cast shadow
			OutLight->bCastShadow = lLight->CastShadows.Get();
		
			// Light type
			switch (lLight->LightType.Get())
			{
			case 0 :
				// Omnidirectional light
				OutLight->LightType = eLightType::OmniDirectional;
				OutLight->AtteunationType = eLightAtteunationType::Quadratic;
				OutLight->SetIntensity(1000.0f);
				break;
			case 1:
			{
				// Directional light
				OutLight->LightType = eLightType::Directional;
				break;
			}
			case 2:
			{
				// Spot light
				OutLight->LightType = eLightType::Spot;
				OutLight->CutOffAngle = lLight->OuterAngle.Get();
				break;
			}
			case 3:
				// Area light
				OutLight->LightType = eLightType::Area;
				break;
			case 4:
				// Volumetric light
				OutLight->LightType = eLightType::Volumetric;
				break;
			default:
				// Invalid light type
				OutLight->LightType = eLightType::InvalidType;
				break;
			}

			// Get the target of the directional light
			auto pTargetNode = pNode->GetTarget();
			if (pTargetNode)
			{
				// Get global position of the node
				FbxAMatrix lTargetNodeGeometryOffset = GetGeometry(pTargetNode);
				FbxAMatrix lTargetNodeGlobalOffPosition = lGlobalPosition * lTargetNodeGeometryOffset;
				OutLight->SetTargetPos(lTargetNodeGlobalOffPosition.GetT()[0], lTargetNodeGlobalOffPosition.GetT()[1], lTargetNodeGlobalOffPosition.GetT()[2]);
			}

			// Set rotation
			auto RotMatrix = glm::rotate(glm::mat4(1.0f), static_cast<float>(glm::radians(lGlobalPosition.GetR()[0])), glm::vec3(1, 0, 0));
			RotMatrix = glm::rotate(RotMatrix, static_cast<float>(glm::radians(lGlobalPosition.GetR()[1])), glm::vec3(0, 1, 0));
			RotMatrix = glm::rotate(RotMatrix, static_cast<float>(glm::radians(lGlobalPosition.GetR()[2])), glm::vec3(0, 0, 1));

			OutLight->SetLightPos(lGlobalPosition.GetT()[0], lGlobalPosition.GetT()[1], lGlobalPosition.GetT()[2]);

			OutSceneContext->Lights.push_back(OutLight);
		}

		return bHasFoundAnyLightNode;
	}

	bool FBXSDKUtil::GetCameraFromRootNode(FbxNode* pNode, std::shared_ptr<SceneContext> OutSceneContext)
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

			OutSceneContext->Cameras.push_back(RetCamera);
		}
		else
		{
			bHasFoundAnyCameraNode = false;
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
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX Manager \n");
			return;
		}
		else
		{
			DEBUG_LOG(eLogType::Verbose, "Autodesk FBX SDK version \n");
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
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX scene \n");
			return;
		}
	}

	void FBXSDKUtil::DestroySDKObjects(FbxManager* pManager, bool pExitStatus)
	{
		// Delete the FBX Manager. All the objects that have been allocated using the FBx Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) DEBUG_LOG(eLogType::Verbose, "Autodesk FBX SDK has been StartToRun successfully \n");
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
			ErrorMsg += '\n';
			DEBUG_LOG(eLogType::Error, ErrorMsg);
			return false;
		}

		FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
		std::string VersionString = "The FBX file format version is " + 
			std::to_string(lMajor) + "." + std::to_string(lMinor) + "." + std::to_string(lRevision);
		VersionString += '\n';
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
			ErrorString += '\n';
			DEBUG_LOG(eLogType::Error, ErrorString);

			if(lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				ErrorString = "FBXSDKHelper::LoadScene, FBX file format version for this FBX SDK is ";
				ErrorString += std::to_string(lSDKMajor) + "." + std::to_string(lSDKMinor) + "." + std::to_string(lSDKRevision);
				ErrorString += "\n";
				DEBUG_LOG(eLogType::Error, ErrorString);
				
				ErrorString = "FBXSDKHelper::LoadScene, While the FBX file format version for file: ";
				ErrorString += pFileName;
				ErrorString += " is " + std::to_string(lFileMajor) + "." + std::to_string(lFileMinor) + "." + std::to_string(lFileRevision);
				ErrorString += "\n";
				DEBUG_LOG(eLogType::Error, ErrorString);
			}

			return false;
		}

		std::string VerboseString = "FBXSDKHelper::LoadScene, FBX file format version for this FBX SDK is ";
		VerboseString += std::to_string(lSDKMajor) + "." + std::to_string(lSDKMinor) + "." + std::to_string(lSDKRevision);
		VerboseString += "\n";
		DEBUG_LOG(eLogType::Verbose, VerboseString);

		if (lImporter->IsFBX())
		{
			VerboseString = "FBXSDKHelper::LoadScene, FBX file format version for file: ";
			VerboseString += std::string(pFileName) + " is " + 
				std::to_string(lFileMajor) + "." + std::to_string(lFileMinor) + "." + std::to_string(lFileRevision);
			VerboseString += '\n';
			DEBUG_LOG(eLogType::Verbose, VerboseString);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.
			
			DEBUG_LOG(eLogType::Verbose, "Animation Stack Information \n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			DEBUG_LOG(eLogType::Verbose, "Number of Animation Stacks: " + std::to_string(lAnimStackCount) + '\n');
			DEBUG_LOG(eLogType::Verbose, "Current Animation Stack: " + (std::string)lImporter->GetActiveAnimStackName() + '\n');

			for (i = 0; i < lAnimStackCount; ++i)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				DEBUG_LOG(eLogType::Verbose, "Animation Stack:" + std::to_string(i) + '\n');
				DEBUG_LOG(eLogType::Verbose, "Name : " + (std::string)lTakeInfo->mName.Buffer() + '\n');
				DEBUG_LOG(eLogType::Verbose, "Animation Stack:" + (std::string)lTakeInfo->mDescription.Buffer() + '\n');

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				DEBUG_LOG(eLogType::Verbose, "Import Name : " + (std::string)lTakeInfo->mImportName.Buffer() + '\n');
				DEBUG_LOG(eLogType::Verbose, "Import State : " + (std::string)(lTakeInfo->mSelect ? "true" : "false") + '\n');
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
			DEBUG_LOG(eLogType::Error, "FBXSDKHelper::SaveScene, Failed to import the scene since it is password protected! \n");
		}

		// Destroy the importer
		lImporter->Destroy();

		return lStatus;
	}

	void FBXSDKUtil::ProcessSceneNode(FbxNode* pRootNode, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext)  noexcept
	{
		if (!pRootNode || !OutSceneContext)
			return;

		auto Name = pRootNode->GetName();

		FbxAMatrix CurrentGlobalPosition = GetGlobalPosition(pRootNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);
		// Get global position of the node
		FbxAMatrix lGeometryOffset = GetGeometry(pRootNode);
		FbxAMatrix lGlobalOffPosition = CurrentGlobalPosition * lGeometryOffset;

		if (pRootNode->GetNodeAttribute() != nullptr)
		{
			auto pWorld = World::GetInstance();
			auto pPhysicalWorld = pWorld->GetPhysicalWorld();
			FbxNodeAttribute::EType AttributeType;
			AttributeType = pRootNode->GetNodeAttribute()->GetAttributeType();

			switch (AttributeType)
			{
			case FbxNodeAttribute::eMesh:
			{
				bool bMeshLoadingRes = FBXSDKUtil::GetMeshFromNode(pRootNode, OutSceneContext, pPhysicalWorld->GetWorldID(), pPhysicalWorld->GetTopSpaceID(), lGlobalOffPosition);
				if (!bMeshLoadingRes)
				{
					DEBUG_LOG(eLogType::Verbose, "SceneManager::ProcessSceneNode, Failed to load the mesh \n");
				}
				break;
			}

			case FbxNodeAttribute::eLight:
			{
				bool bLightLoadingRes = FBXSDKUtil::GetLightFromRootNode(pRootNode, lGlobalOffPosition, OutSceneContext);
				if (!bLightLoadingRes)
				{
					DEBUG_LOG(eLogType::Verbose, "SceneManager::ProcessSceneNode, Failed to load the lights \n");
				}

				break;
			}

			case FbxNodeAttribute::eSkeleton:
			{
				FbxSkeleton* lSkeleton = (FbxSkeleton*)pRootNode->GetNodeAttribute();
				FbxAMatrix lSkeletonGlobalPosition = GetGlobalPosition(pRootNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);

				FBXSDKUtil::LoadSkeletons(pRootNode, lGlobalOffPosition, lSkeletonGlobalPosition, OutSceneContext);
				break;
			}

			case FbxNodeAttribute::eCamera:
			{
				bool bCameraLoadingRes = FBXSDKUtil::GetCameraFromRootNode(pRootNode, OutSceneContext);
				if (!bCameraLoadingRes)
				{
					DEBUG_LOG(eLogType::Verbose, "SceneManager::ProcessSceneNode, Failed to load the Cameras \n");
				}
				break;
			}

			case FbxNodeAttribute::eNull:
			{
				std::string WarningString = "SceneManager::ProcessSceneNode, Find eNull node : ";
				WarningString += pRootNode->GetName();
				WarningString += '\n';
				DEBUG_LOG(eLogType::Warning, WarningString);
				break;
			}
			}
		}

		// Process child node
		auto NodeCount = pRootNode->GetChildCount();
		for (auto i = 0; i < NodeCount; ++i)
		{
			ProcessSceneNode(pRootNode->GetChild(i), lGlobalOffPosition, OutSceneContext);
		}
	}

	glm::mat4 FBXSDKUtil::ConvertFbxMatrixToGLM(const FbxMatrix& InMatrix)
	{
		FbxAMatrix AffineMatrix;
		FbxMatrix TargetMatrix;
		memcpy((double*)AffineMatrix, (double*)TargetMatrix, sizeof(TargetMatrix.mData));

		return ConvertFbxMatrixToGLM(AffineMatrix);
	}

	glm::mat4 FBXSDKUtil::ConvertFbxMatrixToGLM(const FbxAMatrix& InMatrix)
	{
		glm::mat4 RetMatrix(1.0f);

		// Translation
		RetMatrix = glm::translate(RetMatrix, glm::vec3(InMatrix.GetT()[0], InMatrix.GetT()[1], InMatrix.GetT()[2]));

		// Rotation
		RetMatrix = glm::rotate(RetMatrix, static_cast<float>(glm::radians(InMatrix.GetR()[0])), glm::vec3(1, 0, 0));
		RetMatrix = glm::rotate(RetMatrix, static_cast<float>(glm::radians(InMatrix.GetR()[1])), glm::vec3(0, 1, 0));
		RetMatrix = glm::rotate(RetMatrix, static_cast<float>(glm::radians(InMatrix.GetR()[2])), glm::vec3(0, 0, 1));

		// Scaling
		RetMatrix = glm::scale(RetMatrix, glm::vec3(InMatrix.GetS()[0], InMatrix.GetS()[1], InMatrix.GetS()[2]));

		return RetMatrix;
	}

	void FBXSDKUtil::LoadSkeletons(FbxNode* pNode, FbxAMatrix& GlobalParentPosition, FbxAMatrix& GlobalPosition, std::shared_ptr<SceneContext> OutSceneContext)
	{
		FbxSkeleton* lSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();

		glm::vec3 BoneStartPos(GlobalParentPosition.GetT()[0], GlobalParentPosition.GetT()[1], GlobalParentPosition.GetT()[2]);
		glm::vec3 BoneEndPos(GlobalPosition.GetT()[0], GlobalPosition.GetT()[1], GlobalPosition.GetT()[2]);

		auto NewBone = NewObject<CLinkBone>(pNode->GetName(), BoneStartPos, BoneEndPos);

		// Root node
		if (lSkeleton->IsSkeletonRoot())
		{
			std::string SkeletonName;
			if (pNode->GetParent() != pNode->GetScene()->GetRootNode())
				SkeletonName = pNode->GetParent()->GetName();
			else
				SkeletonName = pNode->GetName();

			auto NewSkeleton = std::make_shared<CSkeleton>(SkeletonName);
			NewSkeleton->SetRootBone(NewBone);
			OutSceneContext->Skeletons.push_back(NewSkeleton);
		}
		else
		{
			// Limb node
			// Find the owner skeleton and attach to it
			auto ParentSkeleton = (FbxSkeleton*)pNode->GetParent()->GetNodeAttribute();
			if (!ParentSkeleton)
				DEBUG_LOG(eLogType::Error, "FBXSDKUtil::LoadSkeleton, Parent node is not a skeleton\n");
			else
			{
				bool bHasFoundParentBone = false;
				for (auto pSkeleton : OutSceneContext->Skeletons)
				{
					if (pSkeleton && pSkeleton->AddBone(pNode->GetParent()->GetName(), NewBone))
					{
						// Find the parent bone
						bHasFoundParentBone = true;
						break;
					}
				}

				if(!bHasFoundParentBone)
					DEBUG_LOG(eLogType::Error, "FBXSDKUtil::LoadSkeleton, Can't find the parent bone \n");
			}
		}
	}

	void FBXSDKUtil::LoadPoses(FbxScene* pScene, std::shared_ptr<SceneContext> OutSceneContext)
	{
		if (pScene)
		{
			const int PoseCount = pScene->GetPoseCount();
			for (int i = 0; i < PoseCount; ++i)
			{
				auto NewPose = NewObject<CPose>();
				auto pFbxPose = pScene->GetPose(i);

				NewPose->bIsBindPose = pFbxPose->IsBindPose();
				NewPose->bIsRestPose = pFbxPose->IsRestPose();
				
				auto PoseInfoCount = pFbxPose->GetCount();
				for (int i = 0; i < PoseInfoCount; ++i)
				{
					auto pNewPoseInfo = NewObject<PoseInfo>();
					pNewPoseInfo->bIsLocalMatrix = pFbxPose->IsLocalMatrix(i);
					pNewPoseInfo->PoseMatrix = ConvertFbxMatrixToGLM(pFbxPose->GetMatrix(i));
					pNewPoseInfo->LinkNodeInfo.NodeName = pFbxPose->GetNode(i)->GetName();

					NewPose->AddPoseInfo(pNewPoseInfo);
				}

				OutSceneContext->Poses.push_back(NewPose);
			}
		}
	}

	void FBXSDKUtil::LoadSkinedMeshSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext)
	{
		if (pNode)
		{
			FbxAMatrix CurrentGlobalPosition = GetGlobalPosition(pNode, FBXSDK_TIME_INFINITE, nullptr, &pParentGlobalPosition);
			// Get global position of the node
			FbxAMatrix lGeometryOffset = GetGeometry(pNode);
			FbxAMatrix lGlobalOffPosition = CurrentGlobalPosition * lGeometryOffset;

			if (pNode->GetNodeAttribute())
			{
				auto AttributeType = pNode->GetNodeAttribute()->GetAttributeType();
				if (AttributeType == FbxNodeAttribute::eMesh)
				{
					FbxMesh* pMesh = pNode->GetMesh();
					const bool bHasVertexCache = pMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
						(static_cast<FbxVertexCacheDeformer*>(pMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
					const bool bHasShapeDeformer = pMesh->GetShapeCount() > 0;
					const bool bHasSkinDeformer = pMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
					const bool bHasDeformation = bHasVertexCache || bHasShapeDeformer || bHasSkinDeformer;

					if (bHasDeformation)
					{
						if (bHasVertexCache)
						{
							// TODO : Vertex Animation,
							// 可以搞，但是没必要
						}
						else
						{
							if (bHasShapeDeformer)
							{
								// Shape deformer
							}

							// Skined-mesh skeleton deformation
							const int SkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
							int ClusterCount = 0;
							for (int SkinIndex = 0; SkinIndex < SkinCount; ++SkinIndex)
							{
								FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(SkinIndex, FbxDeformer::eSkin);
								FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

								auto ClusterCount = lSkinDeformer->GetClusterCount();
								for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ++ClusterIndex)
								{
									FbxCluster* lCluster = lSkinDeformer->GetCluster(ClusterIndex);
									FbxNode* pBoneNode = lCluster->GetLink();
									if (!pBoneNode)
										continue;

									std::shared_ptr<CLinkBone> TargetLinkBone = nullptr;
									for (auto pSkeleton : OutSceneContext->Skeletons)
									{
										if (pSkeleton &&
											pBoneNode->GetNodeAttribute() &&
											pBoneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
										{
											TargetLinkBone = pSkeleton->FindBone(pBoneNode->GetName());
										}

										if (TargetLinkBone)
											break;
									}

									if (!TargetLinkBone)
									{
										std::string ErrorString = "FBXSDKUtil::LoadSkinMeshes, Failed to find the Bone the cluster attached to, Link Bone Name = ";
										ErrorString += pBoneNode->GetName();
										ErrorString += '\n';
										DEBUG_LOG(eLogType::Warning, ErrorString);
										continue;
									}

									auto pNewCluster = NewObject<CCluster>();
									pNewCluster->pOwnerBone = TargetLinkBone;

									// Blending mode
									switch (lClusterMode)
									{
									case FbxCluster::eAdditive:
										pNewCluster->ClusterMode = eClusterMode::Additive;
										break;
									case FbxCluster::eNormalize:
										pNewCluster->ClusterMode = eClusterMode::Normalized;
										break;
									case FbxCluster::eTotalOne:
										pNewCluster->ClusterMode = eClusterMode::TotalOne;
										break;
									default:
										break;
									}

									// Get the vertex indices and weights
									int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
									for (int VertexIndex = 0; VertexIndex < lVertexIndexCount; ++VertexIndex)
									{
										pNewCluster->ControlPointsIndices.push_back(lCluster->GetControlPointIndices()[VertexIndex]);
										pNewCluster->ControlPointsWeight.push_back(lCluster->GetControlPointWeights()[VertexIndex]);
									}

									// Add cluster to the bone
									TargetLinkBone->AddCluster(pNewCluster);

									// Assign the mesh that the cluster belongs to
									for (auto pMesh : OutSceneContext->Meshes)
									{
										if (pMesh && pMesh->GetMeshName() == pNode->GetName())
											pNewCluster->SetSkinnedMesh(pMesh);
									}
								}
							}
						}
					}
				}
			}

			// Process the child node
			int ChildNodeCount = pNode->GetChildCount();
			for (int i = 0; i < ChildNodeCount; ++i)
			{
				LoadSkinedMeshSkeleton(pNode->GetChild(i), lGlobalOffPosition, OutSceneContext);
			}
		}
	}

	void FBXSDKUtil::GetNodeNames(FbxNode* pNode, std::vector<std::string>& OutNodeNames)
	{
		if (pNode)
		{
			if (pNode->GetNodeAttribute())
				OutNodeNames.push_back(pNode->GetName());
			else
				auto a = 1;

			for (auto i = 0; i < pNode->GetChildCount(); ++i)
				GetNodeNames(pNode->GetChild(i), OutNodeNames);
		}
	}
}