#ifndef FBXCOMMON_H
#define FBXCOMMON_H

#ifdef WIN32

#include "..\Libraries\FBX-SDK\include\fbxsdk.h"
#include "..\Graphics\Lighting.h"
#include "..\Controller\Camera.h"
#include "..\Graphics\Object3D.h"

#else

#include "../Libraries/FBX-SDk/include/fbxsdk.h"
#include "../Graphics/Lighting.h"
#include "../Controller/Camera.h"
#include "../Graphics/Object3D.h"

#endif

#define GET_MAX(a, b)   (a) < (b) ?  (b) : (a)

namespace cxc
{

	class FBXSDKUtil 
	{

	public:

		explicit FBXSDKUtil();
		~FBXSDKUtil();

	// Light
	public:
		
		static bool GetLightFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<BaseLighting>>& OutLights);

	// Camera 
	public:

		static bool GetCameraFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Camera>>& OutCameras);
		
	// Mesh
	public:

		static bool GetObjectFromNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Object3D>>& OutObjects, std::shared_ptr<Object3D> pParentNode = nullptr);

	private:
		
		static double ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio);

	public:

		static bool LoadRootNodeFromFbxFile(const char* pFileName, /* Out */ FbxNode* RootNode);

	public:
		
		static void InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene);
		static void DestroySDKObjects(FbxManager* pManager, bool pExitStatus);
		//static void CreateAndFillIOSettings(FbxManager* pManager);

		static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat = -1, bool pEmbedMedia = false);
		static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName);
	};
}

#endif // FBXCOMMON_H
