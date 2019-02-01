#ifndef CXC_POSE_H
#define CXC_POSE_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CSkeleton;
	class CLinkBone;

	/* NodeInfo contains the information of the bone or mesh */
	struct NodeInfo
	{
		// Name of the bone
		std::string NodeName;
	};

	/* PoseInfo contains the infomation that the pose has, a CPose instance may has multiple PoseInfo for each bone of a skeleton */
	struct PoseInfo
	{
		// Transform matrix of the pose
		glm::mat4 PoseMatrix;

		// Whether the matrix is local
		bool bIsLocalMatrix;

		// Info of the bone binded to the pose
		NodeInfo LinkNodeInfo;

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

		std::shared_ptr<PoseInfo> FindPoseInfo(const std::string& NodeName);

	private:

		// Whether the pose is a RestPose
		bool bIsRestPose;

		// Whether the pose is BindPose
		bool bIsBindPose;

		// PoseInfos the Pose has
		std::vector<std::shared_ptr<PoseInfo>> PoseInfos;
	};
}

#endif // CXC_POSE_H