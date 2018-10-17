#include "FBXCommon.h"
#include <iostream>

#ifdef IOS_REF
	#undef IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

namespace cxc {

	FBXSDKHelper::FBXSDKHelper()
	{

	}

	FBXSDKHelper::~FBXSDKHelper()
	{

	}

	bool FBXSDKHelper::LoadRootNodeFromFbxFile(const char* pFileName, /* Out */ FbxNode* RootNode)
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

	bool FBXSDKHelper::GetCameraInfoFromRootNode(FbxNode* pNode, /* Out */ CameraInfo& OutCameraInfo)
	{
		if (!pNode)
			return false;

		CameraInfo RetCameraInfo;

		// Get camera from root node
		FbxCamera* lCamera = pNode->GetCamera();

		if (lCamera)
		{

			// Get camera format
			FbxCamera::EFormat lCameraFormat = lCamera->GetFormat();
			double lResolutionWidth; double lResolutionHeight;

			//camera using specific format has a given resolution(aspect) width and height.
			/*
			resolution width            resolution height
			eD1_NTSC                        720                         486
			eNTSC                           640                         480
			ePAL                            570                         486
			eD1_PAL                         720                         576
			eHD                             1980                        1080
			e640x480                        640                         480
			e320x200                        320                         200
			e320x240                        320                         240
			e128x128                        128                         128
			eFULL_SCREEN                    1280                        1024
			*/
			switch (lCameraFormat)
			{
			case FbxCamera::eCustomFormat:
				break;
			case FbxCamera::eNTSC:
				lResolutionWidth = 640;
				lResolutionHeight = 480;
				break;
			case FbxCamera::eD1NTSC:
				lResolutionWidth = 720;
				lResolutionHeight = 486;
				break;
			case FbxCamera::ePAL:
				lResolutionWidth = 570;
				lResolutionHeight = 486;
				break;
			case FbxCamera::eD1PAL:
				lResolutionWidth = 720;
				lResolutionHeight = 576;
				break;
			case FbxCamera::eHD:
				lResolutionWidth = 1980;
				lResolutionHeight = 1080;
				break;
			case FbxCamera::e640x480:
				lResolutionWidth = 640;
				lResolutionHeight = 480;
				break;
			case FbxCamera::e320x200:
				lResolutionWidth = 320;
				lResolutionHeight = 200;
				break;
			case FbxCamera::e320x240:
				lResolutionWidth = 320;
				lResolutionHeight = 240;
				break;
			case FbxCamera::e128x128:
				lResolutionWidth = 128;
				lResolutionHeight = 128;
				break;
			case FbxCamera::eFullscreen:
				lResolutionWidth = 1280;
				lResolutionHeight = 1024;
				break;
			default:
				break;
			}

			// Get camera's inherent properties
			// Aspect height
			double lAspectHeight = lCamera->AspectHeight.Get();
			
			// Aspect width
			double lAspectWidth = lCamera->AspectWidth.Get();

			// Aspect ratio
			double lPixelRatio = lCamera->GetPixelRatio();

			// Verify the pixel ratio
			double lScreenRatio = 4 / 3; // default screen ratio is 4 : 3
			if (lCamera->GetFormat() == FbxCamera::eHD)
			{
				lScreenRatio = 16 / 9; // in HD mode, screen ratio is 16 : 9
			}
			
			double lInspectedPixelRatio = ComputePixelRatio(lResolutionWidth, lResolutionHeight, lScreenRatio);
			if (lPixelRatio != lInspectedPixelRatio)
			{
				std::cerr << "Camera pixel ratio is not correct" << std::endl;
				lCamera->PixelAspectRatio.Set(lInspectedPixelRatio);
				lPixelRatio = lInspectedPixelRatio;
			}
			
			// Get aspect ratio mode
			/*
			If the ratio mode is eWINDOW_SIZE, both width and height values aren't relevant.
			If the ratio mode is eFIXED_RATIO, the height value is set to 1.0 and the width value is relative to the height value.
			If the ratio mode is eFIXED_RESOLUTION, both width and height values are in pixels.
			If the ratio mode is eFIXED_WIDTH, the width value is in pixels and the height value is relative to the width value.
			If the ratio mode is eFIXED_HEIGHT, the height value is in pixels and the width value is relative to the height value.
			*/
			FbxCamera::EAspectRatioMode lCameraAspectRatioMode = lCamera->GetAspectRatioMode();

			// Inspect the aspect width and height
			if (lCameraFormat != FbxCamera::eCustomFormat && lCameraAspectRatioMode != FbxCamera::eWindowSize)
			{
				double lInspectedAspectHeight = 0.0;
				double lInspectedAspectWidth = 0.0;
				switch (lCameraAspectRatioMode)
				{
				default:
					break;
				case FbxCamera::eFixedRatio:
					if (lAspectHeight != 1.0)
					{
						FBXSDK_printf("Camera aspect height should be 1.0 in fixed ratio mode.\n\rRevise the height: %lf to 1.0.\n\n", lAspectHeight);
						lCamera->AspectHeight.Set(1.0);
						lAspectHeight = 1.0;
					}
					lInspectedAspectWidth = lResolutionWidth / lResolutionHeight * lPixelRatio;
					if (lAspectWidth != lInspectedAspectWidth)
					{
						FBXSDK_printf("Camera aspect width is not right.\n\rRevise the width: %lf to %lf.\n\n", lAspectWidth, lInspectedAspectWidth);
						lCamera->AspectWidth.Set(lInspectedAspectWidth);
						lAspectWidth = lInspectedAspectWidth;
					}
					break;
				case FbxCamera::eFixedResolution:
					if (lAspectWidth != lResolutionWidth)
					{
						FBXSDK_printf("Camera aspect width is not right.\n\rRevise the width: %lf to %lf.\n\n", lAspectWidth, lResolutionWidth);
						lCamera->AspectWidth.Set(lResolutionWidth);
						lAspectWidth = lResolutionWidth;
					}
					if (lAspectHeight != lResolutionHeight)
					{
						FBXSDK_printf("Camera aspect height is not right.\n\rRevise the height: %lf to %lf.\n\n", lAspectHeight, lResolutionHeight);
						lCamera->AspectHeight.Set(lResolutionHeight);
						lAspectHeight = lResolutionHeight;
					}
					break;
				case FbxCamera::eFixedWidth:
					lInspectedAspectHeight = lResolutionHeight / lResolutionWidth;
					if (lAspectHeight != lInspectedAspectHeight)
					{
						FBXSDK_printf("Camera aspect height is not right.\n\rRevise the height: %lf to %lf.\n\n", lAspectHeight, lInspectedAspectHeight);
						lCamera->AspectHeight.Set(lInspectedAspectHeight);
						lAspectHeight = lInspectedAspectHeight;
					}
					break;
				case FbxCamera::eFixedHeight:
					lInspectedAspectWidth = lResolutionWidth / lResolutionHeight;
					if (lAspectWidth != lInspectedAspectWidth)
					{
						FBXSDK_printf("Camera aspect width is not right.\n\rRevise the width: %lf to %lf.\n\n", lAspectWidth, lInspectedAspectWidth);
						lCamera->AspectHeight.Set(lInspectedAspectWidth);
						lAspectHeight = lInspectedAspectWidth;
					}
					break;

				}
			}


		}
	}

	//This function computes the pixel ratio
	double  FBXSDKHelper::ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio)
	{
		if (pWidth < 0.0 || pHeight < 0.0)
			return 0.0;

		pWidth = GET_MAX(pWidth, 1.0);
		pHeight = GET_MAX(pHeight, 1.0);

		double lResolutionRatio = (double)pWidth / pHeight;

		return pScreenRatio / lResolutionRatio;
	}

	void FBXSDKHelper::InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene)
	{
		// Check if the FbxManager has already been created
		if (!pManager) return;

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

	void FBXSDKHelper::DestroySDKObjects(FbxManager* pManager, bool pExitStatus)
	{
		// Delete the FBX Manager. All the objects that have been allocated using the FBx Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) std::cout << "Autodesk FBX SDK has been run successfully" << std::endl;
	}

	bool FBXSDKHelper::SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat = -1, bool pEmbedMedia = false)
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

	bool FBXSDKHelper::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

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