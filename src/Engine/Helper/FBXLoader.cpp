#include "FBXloader.h"

FBXLoader::FBXLoader(const char * pFileName, int pWindowWidth, int pWindowHeight)
	: mFileName(pFileName), SdkManager(NULL), Scene(NULL), Importer(NULL), CurrentAnimLayer(NULL), SelectedNode(NULL),
	mPoseIndex(-1)

{
	if (mFileName == NULL)
		mFileName = NULL;

	// initialize cache start and stop time
	mCache_Start = FBXSDK_TIME_INFINITE;
	mCache_Stop = FBXSDK_TIME_MINUS_INFINITE;

	// Create the FBX SDK manager which is the object allocator for almost 
	// all the classes in the SDK and create the scene.
	InitializeSdkObjects(SdkManager, Scene);

	if (SdkManager)
	{
		// Create the importer.
		int lFileFormat = -1;
		Importer = FbxImporter::Create(SdkManager, "");
		if (!SdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(mFileName, lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = SdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		// Initialize the importer by providing a filename.
		if (Importer->Initialize(mFileName, lFileFormat) == true)
		{
			// The file is going to be imported at 
			// the end of the first display callback.
			WindowMessage = "Importing file ";
			WindowMessage += mFileName;
			WindowMessage += "\nPlease wait!";

		}
		else
		{
			WindowMessage = "Unable to open file ";
			WindowMessage += mFileName;
			WindowMessage += "\nError reported: ";
			WindowMessage += Importer->GetStatus().GetErrorString();
			WindowMessage += "\nEsc to exit";
		}
	}
	else
	{
		WindowMessage = "Unable to create the FBX SDK manager";
		WindowMessage += "\nEsc to exit";
	}

}

FBXLoader::~FBXLoader(void)
{

	FbxArrayDelete(mAnimStackNameArray);

	// Unload the cache and free the memory
	if (Scene)
	{
		UnloadCacheRecursive(Scene);
	}

	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	DestroySdkObjects(SdkManager, true);
}

void  FBXLoader::FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
{
	const int lPoseCount = pScene->GetPoseCount();

	for (int i = 0; i < lPoseCount; ++i)
	{
		pPoseArray.Add(pScene->GetPose(i));
	}
}

void FBXLoader::PreparePointCacheData(FbxScene* pScene, FbxTime &pCache_Start, FbxTime &pCache_Stop)
{

	// This function show how to cycle through scene elements in a linear way.
	const int lNodeCount = pScene->GetSrcObjectCount<FbxNode>();
	FbxStatus lStatus;

	for (int lIndex = 0; lIndex<lNodeCount; lIndex++)
	{
		FbxNode* lNode = pScene->GetSrcObject<FbxNode>(lIndex);

		if (lNode->GetGeometry())
		{
			int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(FbxDeformer::eVertexCache);

			// There should be a maximum of 1 Vertex Cache Deformer for the moment
			lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

			for (i = 0; i<lVertexCacheDeformerCount; ++i)
			{
				// Get the Point Cache object
				FbxVertexCacheDeformer* lDeformer = static_cast<FbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, FbxDeformer::eVertexCache));
				if (!lDeformer) continue;
				FbxCache* lCache = lDeformer->GetCache();
				if (!lCache) continue;

				// Process the point cache data only if the constraint is active
				if (lDeformer->Active)
				{
					if (lCache->GetCacheFileFormat() == FbxCache::eMaxPointCacheV2)
					{
						// This code show how to convert from PC2 to MC point cache format
						// turn it on if you need it.
#if 0 
						if (!lCache->ConvertFromPC2ToMC(FbxCache::eMCOneFile,
							FbxTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
						{
							// Conversion failed, retrieve the error here
							FbxString lTheErrorIs = lCache->GetStaus().GetErrorString();
						}
#endif
					}
					else if (lCache->GetCacheFileFormat() == FbxCache::eMayaCache)
					{
						// This code show how to convert from MC to PC2 point cache format
						// turn it on if you need it.
						//#if 0 
						if (!lCache->ConvertFromMCToPC2(FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), 0, &lStatus))
						{
							// Conversion failed, retrieve the error here
							FbxString lTheErrorIs = lStatus.GetErrorString();
						}
						//#endif
					}


					// Now open the cache file to read from it
					if (!lCache->OpenFileForRead(&lStatus))
					{
						// Cannot open file 
						FbxString lTheErrorIs = lStatus.GetErrorString();

						// Set the deformer inactive so we don't play it back
						lDeformer->Active = false;
					}
					else
					{
						// get the start and stop time of the cache
						int lChannelCount = lCache->GetChannelCount();

						for (int iChannelNo = 0; iChannelNo < lChannelCount; iChannelNo++)
						{
							FbxTime lChannel_Start;
							FbxTime lChannel_Stop;

							if (lCache->GetAnimationRange(iChannelNo, lChannel_Start, lChannel_Stop))
							{
								// get the smallest start time
								if (lChannel_Start < pCache_Start) pCache_Start = lChannel_Start;

								// get the biggest stop time
								if (lChannel_Stop  > pCache_Stop)  pCache_Stop = lChannel_Stop;
							}
						}
					}
				}
			}
		}
	}

}

void FBXLoader::LoadCacheRecursive(FbxNode * pNode, FbxAnimLayer * pAnimLayer)
{

	// Bake material and hook as user data.
	const int lMaterialCount = pNode->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
		if (lMaterial && !lMaterial->GetUserDataPtr())
		{
			FbxAutoPtr<MaterialCache> lMaterialCache(new MaterialCache);
			if (lMaterialCache->Initialize(lMaterial))
			{
				lMaterial->SetUserDataPtr(lMaterialCache.Release());
			}
		}
	}

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		// Bake mesh as VBO(vertex buffer object) into GPU.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
			if (lMesh && !lMesh->GetUserDataPtr())
			{
				FbxAutoPtr<VBOMesh> lMeshCache(new VBOMesh);
				if (lMeshCache->Initialize(lMesh))
				{
					lMesh->SetUserDataPtr(lMeshCache.Release());
				}
			}
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		LoadCacheRecursive(pNode->GetChild(lChildIndex), pAnimLayer);
	}

}

void FBXLoader::UnloadCacheRecursive(FbxNode * pNode)
{
	// Unload the material cache
	const int lMaterialCount = pNode->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
		if (lMaterial && lMaterial->GetUserDataPtr())
		{
			MaterialCache * lMaterialCache = static_cast<MaterialCache *>(lMaterial->GetUserDataPtr());
			lMaterial->SetUserDataPtr(NULL);
			delete lMaterialCache;
		}
	}

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		// Unload the mesh cache
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
			if (lMesh && lMesh->GetUserDataPtr())
			{
				VBOMesh * lMeshCache = static_cast<VBOMesh *>(lMesh->GetUserDataPtr());
				lMesh->SetUserDataPtr(NULL);
				delete lMeshCache;
			}
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		UnloadCacheRecursive(pNode->GetChild(lChildIndex));
	}
}

void FBXLoader::LoadCacheRecursive(FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName)
{
	// Load the textures into GPU, only for file texture now
	const int lTextureCount = pScene->GetTextureCount();
	for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
	{
		FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
		FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
		if (lFileTexture && !lFileTexture->GetUserDataPtr())
		{
			// Try to load the texture from absolute path
			const FbxString lFileName = lFileTexture->GetFileName();

			GLuint lTextureObject = 0;
			bool lStatus = LoadTextureFromFile(lFileName, lTextureObject); // <<---texture

			const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
			const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
			if (!lStatus)
			{
				// Load texture from relative file name (relative to FBX file)
				const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
				lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);// <<---texture
			}

			if (!lStatus)
			{
				// Load texture from file name only (relative to FBX file)
				const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
				const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
				lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject); // <<------texture
			}

			if (!lStatus)
			{
				FBXSDK_printf("Failed to load texture file: %s\n", lFileName.Buffer());
				continue;
			}

			if (lStatus)
			{
				GLuint * lTextureName = new GLuint(lTextureObject);
				lFileTexture->SetUserDataPtr(lTextureName);
			}
		}
	}

	LoadCacheRecursive(pScene->GetRootNode(), pAnimLayer);
}

