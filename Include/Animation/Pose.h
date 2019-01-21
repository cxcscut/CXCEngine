#ifndef CXC_POSE_H
#define CXC_POSE_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CSkeleton;
	class CLinkBone;

	/* BoneInfo contains the information of the bone */
	struct BoneInfo
	{
		// Name of the bone
		std::string BoneName;
	};

	/* PoseInfo contains the infomation that the pose has, a CPose instance may has multiple PoseInfo for each bone of a skeleton */
	struct PoseInfo
	{
		// Transform matrix of the pose
		glm::mat4 PoseMatrix;

		// Whether the matrix is local
		bool bIsLocalMatrix;

		// Info of the bone binded to the pose
		BoneInfo LinkBoneInfo;

	};

	/* CPose is the class containing the description of a pose that a skeleton has */
	class CXC_ENGINECORE_API CPose
	{
	public:
		friend class FBXSDKUtil;

		CPose();
		~CPose();

	public:

		uint32_t GetPoseInfoCount() const { return PoseInfos.size(); }
		std::shared_ptr<PoseInfo> GetPoseInfo(uint32_t Index) const;

		bool IsBindPose() const { return bIsBindPose; }
		bool IsRestPose() const { return bIsRestPose; }

		void AddPoseInfo(std::shared_ptr<PoseInfo> pNewInfo);
		void RemovePoseInfo(uint32_t Index);

		std::shared_ptr<PoseInfo> FindBoneInfo(const std::string& BoneName);

	private:

		// Whether the pose is a RestPose
		bool bIsRestPose;

		// Whether the pose is BindPose
		bool bIsBindPose;

		// Skeleton that the pose is binded to
		std::shared_ptr<CSkeleton> pOwnerSkeleton;

		// PoseInfos the Pose has
		std::vector<std::shared_ptr<PoseInfo>> PoseInfos;
	};
}

#endif // CXC_POSE_H