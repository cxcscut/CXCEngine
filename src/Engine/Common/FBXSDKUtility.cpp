#include "FBXSDKUtility.h"

#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif

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

	bool FBXSDKUtil::LoadRootNodeFromFbxFile(const char* pFileName, /* Out */ FbxNode* RootNode)
	{
		FbxManager* lSdkManager = nullptr;
		FbxScene* lScene = nullptr;
		bool lResult;

		// Prepare the FBX SDK
		InitializeSDKObjects(lSdkManager, lScene);

		// Load the Scene
		std::cout << "Loading the FBX file " << pFileName << std::endl;
		
		lResult = LoadScene(lSdkManager, lScene, pFileName);
		if (lResult)
		{
			if (lScene)
			{
				// Get root node of the fbx scene
				RootNode = lScene->GetRootNode();

				return RootNode == nullptr ? false : true;
			}
			else
			{
				std::cerr << "FBXSDKHelper::LoadRootNodeFromFbxFile, the lScene is nullptr" << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "FBXSDKHelper::LoadRootNodeFromFbxFile, error occured while loading the scene" << std::endl;
			return false;
		}
	}

	bool FBXSDKUtil::GetObjectFromNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Object3D>>& OutObjects, std::shared_ptr<Object3D> pParentNode)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyObject = false;
		std::shared_ptr<Object3D> pNewObject = nullptr;

		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh)
		{
			const int lPolygonCount = pMesh->GetPolygonCount();
			bHasFoundAnyObject = true;

			// Count the polygon count of each material
			FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
			FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
			if (pMesh->GetElementMaterial())
			{
				lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
				lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();

				// Do not support material per face
				if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eMaterial)
				{

				}
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
			std::map<VertexIndexPacket, uint32_t> VertexIndexingMap;

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
							static_cast<float>(CurrentUV[1])
						));
					}
				}
			}

			for (int PolygonIndex = 0; PolygonIndex < lPolygonCount; ++PolygonIndex)
			{
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
								static_cast<float>(CurrentUV[1])
							);
						}

						VertexIndexPacket VertexPacket(VertexPos, VertexNormal, VertexUV);
						auto iter = VertexIndexingMap.find(VertexPacket);
						if (iter != VertexIndexingMap.end())
						{
							Indices.push_back(iter->second);
						}
						else
						{
							Vertices.emplace_back(VertexPos);
							Normals.emplace_back(VertexNormal);
							UVs.emplace_back(VertexUV);
							Indices.push_back(Vertices.size() - 1);

							VertexIndexingMap.insert(std::make_pair(VertexPacket, Vertices.size() - 1));
						}
					}
				}
			}

			// Create the shape
			pNewObject = std::make_shared<Object3D>(Vertices, Normals, UVs, Indices);
			pNewObject->ObjectName = pNode->GetName();
			pNewObject->isLoaded = true;

			// Create the parent-child relationship
			if (pParentNode)
			{
				pParentNode->pChildNodes.push_back(pNewObject);
				pNewObject->pParentNode = pParentNode;
			}

			OutObjects.push_back(pNewObject);
		}

		// Recursively traverse each node in the scene
		int i, lCount = pNode->GetChildCount();
		for (i = 0; i < lCount; i++)
		{
			bHasFoundAnyObject |= GetObjectFromNode(pNode->GetChild(i), OutObjects, pNewObject);
		}

		return bHasFoundAnyObject;
	}

	bool FBXSDKUtil::GetLightFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<BaseLighting>>& OutLights)
	{
		if (!pNode)
			return false;

		bool bHasFoundAnyLightNode = false;

		std::shared_ptr<BaseLighting> RetLight = std::make_shared<BaseLighting>();

		// Get light from root node
		FbxLight* lLight = pNode->GetLight();
		if (lLight)
		{
			// Marked as successful
			bHasFoundAnyLightNode = true;

			/* Save the light configuration */
			// Light Name
			RetLight->LightName = lLight->GetName();

			// Light color
			FbxDouble3 LightColor = lLight->Color.Get();
			RetLight->LightColor = glm::vec3(LightColor[0], LightColor[1], LightColor[2]);

			// Light intensity
			RetLight->LightIntensity = lLight->Intensity.Get();

			// Whether to cast light
			RetLight->bCastLight = lLight->CastLight.Get();

			// Whether to cast shadow
			RetLight->bCastShadow = lLight->CastShadows.Get();

			// Light type
			switch (lLight->LightType.Get())
			{
			case 0 :
				// Omnidirectional light
				RetLight->LightType = eLightType::OmniDirectional;
				break;
			case 1:
				// Directional light
				RetLight->LightType = eLightType::Directional;
				break;
			case 2:
				// Spot light
				RetLight->LightType = eLightType::Spot;
				break;
			case 3:
				// Area light
				RetLight->LightType = eLightType::Area;
				break;
			case 4:
				// Volumetric light
				RetLight->LightType = eLightType::Volumetric;
				break;
			default:
				// Invalid light type
				RetLight->LightType = eLightType::InvalidType;
				break;
			}

			// Get global position and orientation
			FbxAMatrix lGlobalTransform = pNode->EvaluateGlobalTransform();
			const FbxVector4 lTranslation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
			const FbxVector4 lRotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
			const FbxVector4 lScale = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix GlobalOffsetPosition = lGlobalTransform * FbxAMatrix(lTranslation, lRotation, lScale);
			RetLight->LightPos = glm::vec3(GlobalOffsetPosition.GetT()[0], GlobalOffsetPosition.GetT()[1], GlobalOffsetPosition.GetT()[2]);
			RetLight->LightDirection = glm::vec3(GlobalOffsetPosition.GetR()[0], GlobalOffsetPosition.GetR()[1], GlobalOffsetPosition.GetR()[2]);

			OutLights.emplace_back(RetLight);
		}

		// Recursively traverse each node in the scene
		int i, lCount = pNode->GetChildCount();
		for (i = 0; i < lCount; i++)
		{
			bHasFoundAnyLightNode |= GetLightFromRootNode(pNode->GetChild(i), OutLights);
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
			std::cerr << "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX Manager" << std::endl;
			return;
		}
		else
		{
			std::cout << "Autodesk FBX SDK version " << pManager->GetVersion() << std::endl;
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
			std::cerr << "FBXSDKHelper::InitializeSDKObjects, Unable to create FBX scene" << std::endl;
			return;
		}
	}

	void FBXSDKUtil::DestroySDKObjects(FbxManager* pManager, bool pExitStatus)
	{
		// Delete the FBX Manager. All the objects that have been allocated using the FBx Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) std::cout << "Autodesk FBX SDK has been run successfully" << std::endl;
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
			std::cerr << "FBXSDKHelper::SaveScene, failed to initialize the FbxExporter " 
				<<  "with the errors returned : " << lExporter->GetStatus().GetErrorString()
				<<	std::endl;
			return false;
		}

		FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
		std::cout << "the FBX file format version is "
			<< lMajor << "." << lMinor << "." << lRevision << std::endl;

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
			FbxString error = lImporter->GetStatus().GetErrorString();
			std::cerr << "FBXSDKHelper::LoadScene, failed to initializer the FbxImporter " 
				<< "with the errors returned " << error.Buffer()
				<< std::endl;

			if(lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				std::cerr << "FBX file format version for this FBX SDK is" 
					<< lSDKMajor << "." << lSDKMinor << "." << lSDKRevision
					<< std::endl;
				std::cerr << "While the FBX file format version for file: " 
					<< pFileName << " is" << lFileMajor << "." << lFileMinor << "." << lFileRevision 
					<< std::endl;
			}

			return false;
		}

		std::cout << "FBX file format version for this FBX SDK is"
			<< lSDKMajor << "." << lSDKMinor << "." << lSDKRevision
			<< std::endl;

		if (lImporter->IsFBX())
		{
			std::cout << "FBX file format version for file: "
				<< pFileName << " is" << lFileMajor << "." << lFileMinor << "." << lFileRevision
				<< std::endl;

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.
			
			std::cout << "Animation Stack Information" << std::endl;

			lAnimStackCount = lImporter->GetAnimStackCount();

			std::cout << "Number of Animation Stacks: " << lAnimStackCount << std::endl;
			std::cout << "Current Animation Stack: " << lImporter->GetActiveAnimStackName().Buffer() << std::endl;

			for (i = 0; i < lAnimStackCount; ++i)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				std::cout << "Animation Stack: " << i << std::endl;
				std::cout << "Name : " << lTakeInfo->mName.Buffer() << std::endl;
				std::cout << "Description : " << lTakeInfo->mDescription.Buffer() << std::endl;

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				std::cout << "Import Name : " << lTakeInfo->mImportName.Buffer() << std::endl;
				std::cout << "Import State : " << (lTakeInfo->mSelect ? "true" : "false") << std::endl;
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
			std::cerr << "Failed to import the scene since it is password protected!" << std::endl;
		}

		// Destroy the importer
		lImporter->Destroy();

		return lStatus;
	}
}