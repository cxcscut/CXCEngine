#ifndef FBXCOMMON_H
#define FBXCOMMON_H

#ifdef WIN32

#include "..\Libraries\FBX-SDK\include\fbxsdk.h"

#else

#include "../Libraries/FBX-SDk/include/fbxsdk.h"

#endif

#define GET_MAX(a, b)   (a) < (b) ?  (b) : (a)

namespace cxc
{

	class FBXSDKHelper 
	{

	public:

		explicit FBXSDKHelper();
		~FBXSDKHelper();

	// Camera 
	public:

		using CameraInfo = struct CameraInfo {

			// Resolution of the camera
			double ResolutionHeight;
			double ResolutionWidth;

			// Aspect of the camera
			double AspectHeight;
			double AspectWidth;
			double PixelRatio;
			

		};

		static bool GetCameraInfoFromRootNode(FbxNode* pNode, /* Out */ CameraInfo& OutCameraInfo);
		
	private:

		static double ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio);

	public:

		static bool LoadRootNodeFromFbxFile(const char* pFileName, /* Out */ FbxNode* RootNode);

	private:
		
		static void InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene);
		static void DestroySDKObjects(FbxManager* pManager, bool pExitStatus);
		//static void CreateAndFillIOSettings(FbxManager* pManager);

		static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat = -1, bool pEmbedMedia = false);
		static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName);
	};
}

#endif // FBXCOMMON_H
