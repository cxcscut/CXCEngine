#ifndef CXC_PAWN_H
#define CXC_PAWN_H

#include "Actor/CActor.h"
#include "Physics/RigidBody3D.h"

namespace cxc
{
	/* CPawn is an CActor that can simulate physics */
	class CPawn : public CActor , public RigidBody3D
	{
	public :
		CPawn();
		CPawn(const std::string& Name, const std::string& Tag = "");
		virtual ~CPawn();

	public:

		virtual void Translate(const glm::vec3 &TranslationVector) override;
		virtual void RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree) override;
		virtual void RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree) override;
		virtual void RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree) override;
		virtual void Scale(const glm::vec3& ScalingVector) override;

		virtual glm::mat4 GetModelMatrix() const override;

		virtual void Tick(float DeltaSeconds) override;

	public:

		virtual void PhysicalTick(float DeltaSeconds);

	private:

		void InitializeRigidBody();
	};
}

#endif // CXC_PAWN_H