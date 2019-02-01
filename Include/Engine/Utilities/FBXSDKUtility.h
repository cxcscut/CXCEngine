#ifndef FBXCOMMON_H
#define FBXCOMMON_H

#include "fbxsdk.h"
#include "Geometry/Mesh.h"
#include "Scene/Lighting.h"
#include "Scene/Camera.h"
#include "Actor/CPawn.h"
#include "Material/TextureManager.h"

#define GET_MAX(a, b)   (a) < (b) ?  (b) : (a)

namespace cxc
{
	class AnimStack;
	class AnimLayer;
	class AnimCurve;
	class SceneContext;
	class CSkeleton;
	class CLinkBone;
	class CPose;

	class CXC_ENGINECORE_API FBXSDKUtil final
	{

	public:

		explicit FBXSDKUtil();
		~FBXSDKUtil();

	// Skeleton
	public:
		static void GetNodeNames(FbxNode* pNode, std::vector<std::string>& OutNodeNames);

		static void LoadSkinedMeshSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext);
		static void LoadSkeletons(FbxNode* pNode, FbxAMatrix& GlobalParentPosition, FbxAMatrix& GlobalPosition, std::shared_ptr<SceneContext> OutSceneContext);
		static void LoadPoses(FbxScene* pScene, std::shared_ptr<SceneContext> OutSceneContext);

	// Animation
	public:

		static void LoadAnimationStacks(FbxScene* pScene, std::shared_ptr<SceneContext> OutSceneContext);
		static void ExtractAnimStack(FbxAnimStack* pAnimStack, FbxNode* pNode, std::shared_ptr<AnimStack> OutAnimStack);
		static void ExtractAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode, std::shared_ptr<AnimLayer> OutAnimLayer, bool bIsSwitcher =false);
		static std::shared_ptr<AnimCurve> ExtractAnimKeyFrames(FbxNode* pNode, FbxAnimCurve* pCurve);
		static std::shared_ptr<AnimCurve> ExtractListAnimKeyFrames(FbxNode* pNode, FbxAnimCurve* pCurve, FbxProperty* pProperty);

		static int InterpolationFlagToIndex(int flags);
		static int ConstantmodeFlagToIndex(int flags);
		static int TangentmodeFlagToIndex(int flags);
		static int TangentweightFlagToIndex(int flags);
		static int TangentVelocityFlagToIndex(int flags);

	// Light
	public:
		
		static bool GetLightFromRootNode(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext);

	// Camera 
	public:

		static bool GetCameraFromRootNode(FbxNode* pNode, std::shared_ptr<SceneContext> OutSceneContext);
		
	// SubMesh
	public:

		static void GetMaterialProperties(const FbxSurfaceMaterial* pMaterial, 
			glm::vec3& Emissive, glm::vec3& Ambient, glm::vec3& Diffuse, glm::vec3& Specular, 
			GLuint& EmissiveTexName, GLuint& AmbientTexName, GLuint& DiffuseTexName, GLuint& SpecularTexName,
			float& Shiniess);

		static FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
			const char * pPropertyName,
			const char * pFactorPropertyName,
			GLuint& pTextureName);

		static bool GetMeshFromNode(FbxNode* pNode, std::shared_ptr<SceneContext> OutSceneContext, dWorldID WorldID, dSpaceID SpaceID, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<Mesh> pParentNode = nullptr);
		static void GetTexturesFromMaterial(FbxSurfaceMaterial* pSurfaceMaterial, std::vector<std::shared_ptr<Texture2D>>& OutTextures);

	private:
		
		static double ComputePixelRatio(double pWidth, double pHeight, double pScreenRatio);

	public:

		static FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = nullptr, FbxAMatrix* pParentGlobalPosition = nullptr);
		static FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
		static FbxAMatrix GetGeometry(FbxNode* pNode);
		static glm::mat4 ConvertFbxMatrixToGLM(const FbxAMatrix& InMatrix);
		static glm::mat4 ConvertFbxMatrixToGLM(const FbxMatrix& InMatrix);

	public:
		
		static void InitializeSDKObjects(FbxManager*& pManager, FbxScene*& pScene);
		static void DestroySDKObjects(FbxManager* pManager, bool pExitStatus);
		//static void CreateAndFillIOSettings(FbxManager* pManager);

		static bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName, int pFileFormat = -1, bool pEmbedMedia = false);
		static bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFileName);
		static void ProcessSceneNode(FbxNode* pRootNode, FbxAMatrix& pParentGlobalPosition, std::shared_ptr<SceneContext> OutSceneContext)  noexcept;
	};
}

#endif // FBXCOMMON_H
