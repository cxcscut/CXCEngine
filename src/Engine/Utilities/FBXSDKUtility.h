#ifndef FBXCOMMON_H
#define FBXCOMMON_H

#ifdef WIN32

#include "..\Libraries\FBX-SDK\include\fbxsdk.h"
#include "..\Scene\Lighting.h"
#include "..\Scene\Camera.h"
#include "..\Scene\Object3D.h"
#include "..\Material\TextureManager.h"

#else

#include "../Libraries/FBX-SDk/include/fbxsdk.h"
#include "../Scene/Lighting.h"
#include "../Scene/Camera.h"
#include "../Scene/Object3D.h"
#include "../Rendering/TextureManager.h"

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
		
		static bool GetLightFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<BaseLighting>>& OutLights, FbxAMatrix& pParentGlobalPosition);

	// Camera 
	public:

		static bool GetCameraFromRootNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Camera>>& OutCameras);
		
	// Mesh
	public:

		static void GetMaterialProperties(const FbxSurfaceMaterial* pMaterial, 
			glm::vec3& Emissive, glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular, 
			GLuint& EmissiveTexName, GLuint& AmbientTexName, GLuint& DiffuseTexName, GLuint& SpecularTexName,
			float& Shiniess);

		static FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
			const char * pPropertyName,
			const char * pFactorPropertyName,
			GLuint& pTextureName);

		static bool GetObjectFromNode(FbxNode* pNode, /* Out */ std::vector<std::shared_ptr<Object3D>>& OutObjects, dWorldID WorldID, dSpaceID SpaceID, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<Object3D> pParentNode = nullptr);
		static void GetTexturesFromMaterial(FbxSurfaceMaterial* pSurfaceMaterial, std::vector<std::shared_ptr<Texture2D>>& OutTextures);

	private:
		
		static double ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio);

	public:

		static FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = nullptr, FbxAMatrix* pParentGlobalPosition = nullptr);
		static FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
		static FbxAMatrix GetGeometry(FbxNode* pNode);

	public:
		
		static void InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene);
		static void DestroySDKObjects(FbxManager* pManager, bool pExitStatus);
		//static void CreateAndFillIOSettings(FbxManager* pManager);

		static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat = -1, bool pEmbedMedia = false);
		static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName);
	};
}

#endif // FBXCOMMON_H
