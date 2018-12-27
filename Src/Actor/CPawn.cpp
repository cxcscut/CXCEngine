#include "Actor/CPawn.h"
#include "World/World.h"

namespace cxc
{
	CPawn::CPawn():
		CActor()
	{
		InitializeRigidBody();
	}

	CPawn::~CPawn()
	{

	}

	CPawn::CPawn(const std::string &name, const std::string& Tag)
		: CActor(name)
	{
		InitializeRigidBody();
	}

	void CPawn::Tick(float DeltaSeconds)
	{
		CActor::Tick(DeltaSeconds);
	}

	void CPawn::InitializeRigidBody()
	{
		auto StaticMeshComponent = GetComponent<CStaticMeshComponent>();

		if (StaticMeshComponent)
		{
			auto pMesh = StaticMeshComponent->GetStaticMesh();
			auto pWorld = World::GetInstance();
			auto pPhysicalWorld = pWorld->GetPhysicalWorld();
			createRigidBody(pPhysicalWorld->GetWorldID(), reinterpret_cast<void*>(this));

			addCollider(pPhysicalWorld->GetTopSpaceID(), pMesh->m_VertexCoords, pMesh->m_VertexIndices);

			if (isKinematics)
				dBodySetKinematic(GetBodyID());

			Initialized = true;
		}
	}

	void CPawn::PhysicalTick(float DeltaSeconds)
	{
		UpdateMeshTransform();
	}

	void CPawn::Translate(const glm::vec3 &TranslationVector)
	{
		auto new_pos = getPosition() + TranslationVector;
		setPosition(new_pos.x, new_pos.y, new_pos.z);
	}

	void CPawn::RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisWorldSpace);
		setRotation(glm::mat3(RotMatrix) * getRotation());
	}

	void CPawn::RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisLocalSpace);
		setRotation(getRotation() * glm::mat3(RotMatrix));
	}

	void CPawn::RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree)
	{
		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 TransToOrigin = glm::translate(glm::mat4(1.0f), -Position);
		glm::mat4 RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxis);
		glm::mat4 TransBack = glm::translate(glm::mat4(1.0f), Position);

		 auto ChainMatrix = TransBack * RotMatrix * TransToOrigin;

		 setRotation(glm::mat3(ChainMatrix) * getRotation());

		 auto Pos = getPosition();

		 // Set position vector
		 setPosition(
			 ChainMatrix[0][0] * Pos.x + ChainMatrix[1][0] * Pos.y + ChainMatrix[2][0] * Pos.z + ChainMatrix[3][0],
			 ChainMatrix[0][1] * Pos.x + ChainMatrix[1][1] * Pos.y + ChainMatrix[2][1] * Pos.z + ChainMatrix[3][1],
			 ChainMatrix[0][2] * Pos.x + ChainMatrix[1][2] * Pos.y + ChainMatrix[2][2] * Pos.z + ChainMatrix[3][2]
		 );

		 TransformMatrix = ChainMatrix * TransformMatrix;
	}

	void CPawn::Scale(const glm::vec3& ScalingVector)
	{
		SetScalingFactor(GetScalingFactor() * ScalingVector);
	}

	glm::mat4 CPawn::GetModelMatrix() const
	{
		return GetRigidBodyModelMatrix();
	}
}