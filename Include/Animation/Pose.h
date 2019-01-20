#ifndef CXC_POSE_H
#define CXC_POSE_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CSkeleton;
	class CLinkBone;

	/* PoseInfo contains the infomation that the pose has, a CPose instance may has multiple PoseInfo for each bone of a skeleton */
	struct PoseInfo
	{
		// Transform matrix of the pose
		glm::mat4 PoseMatrix;

		// Whether the matrix is local
		bool bIsLocalMatrix;

		// Bones that the pose is binded
		std::shared_ptr<CLinkBone> TargetBone;
	};

	/* CPose is the class containing the description of a pose that a skeleton has */
	class CXC_ENGINECORE_API CPose
	{
	public:
		CPose();
		~CPose();

	public:

		uint32_t GetPoseInfoCount() const { return PoseInfos.size(); }
		const PoseInfo& GetPoseInfo(uint32_t Index) const;

		bool IsBindPose() const { return bIsBindPose; }
		bool IsRestPose() const { return bIsRestPose; }

	private:

		// Whether the pose is a RestPose
		bool bIsRestPose;

		// Whether the pose is BindPose
		bool bIsBindPose;

		// Skeleton that the pose is binded to
		std::shared_ptr<CSkeleton> pOwnerSkeleton;

		// PoseInfos the Pose has
		std::vector<PoseInfo> PoseInfos;
	};
}

#endif // CXC_POSE_H