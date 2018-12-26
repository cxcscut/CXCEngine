#include "Scene/Pawn.h"
#include "World/World.h"

namespace cxc
{
	Pawn::Pawn():
		Object3D()
	{
		InitializeRigidBody();
	}

	Pawn::~Pawn()
	{

	}

	Pawn::Pawn(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals) :
		Object3D(Vertices, Normals)
	{
		InitializeRigidBody();
	}

	Pawn::Pawn(std::vector<glm::vec3>& Vertices,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& UVs,
		std::vector<uint32_t>& Indices)
		: Object3D(Vertices, Normals, UVs, Indices)
	{
		InitializeRigidBody();
	}

	Pawn::Pawn(const std::string &name)
		: Object3D(name)
	{
		InitializeRigidBody();
	}

	Pawn::Pawn(const std::string &name, const std::string &filename, const std::string &_tag, GLboolean _enable)
		: Object3D(name, filename, _tag, _enable)
	{
		InitializeRigidBody();
	}

	void Pawn::Tick(float DeltaSeconds)
	{
		Object3D::Tick(DeltaSeconds);

	}

	void Pawn::InitializeRigidBody()
	{
		auto pWorld = World::GetInstance();
		auto pPhysicalWorld = pWorld->GetPhysicalWorld();
		createRigidBody(pPhysicalWorld->GetWorldID(), reinterpret_cast<void*>(this));

		addCollider(pPhysicalWorld->GetTopSpaceID(), m_VertexCoords, m_VertexIndices);

		if (isKinematics)
			dBodySetKinematic(GetBodyID());

		Initialized = true;
	}

	void Pawn::PhysicalTick(float DeltaSeconds)
	{
		UpdateMeshTransform();
	}

	void Pawn::Translate(const glm::vec3 &TranslationVector)
	{
		auto new_pos = getPosition() + TranslationVector;

		setPosition(new_pos.x, new_pos.y, new_pos.z);

		// Perform translation on the child node
		for (auto &pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->Translate(TranslationVector);
		}
	}

	void Pawn::RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisWorldSpace);

		setRotation(glm::mat3(RotMatrix) * getRotation());

		// Perform translation on the child node
		for (auto &pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->RotateWorldSpace(RotationAxisWorldSpace, Degree);
		}
	}

	void Pawn::RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisLocalSpace);

		setRotation(getRotation() * glm::mat3(RotMatrix));

		// Perform translation on the child node
		for (auto &pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->RotateLocalSpace(RotationAxisLocalSpace, Degree);
		}
	}

	void Pawn::RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree)
	{
		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 TransToOrigin = glm::translate(glm::mat4(1.0f), -Position);
		glm::mat4 RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxis);
		glm::mat4 TransBack = glm::translate(glm::mat4(1.0f), Position);

		 auto TransformMatrix = TransBack * RotMatrix * TransToOrigin;

		 setRotation(glm::mat3(TransformMatrix) * getRotation());

		 auto Pos = getPosition();

		 // Set position vector
		 setPosition(
			 TransformMatrix[0][0] * Pos.x + TransformMatrix[1][0] * Pos.y + TransformMatrix[2][0] * Pos.z + TransformMatrix[3][0],
			 TransformMatrix[0][1] * Pos.x + TransformMatrix[1][1] * Pos.y + TransformMatrix[2][1] * Pos.z + TransformMatrix[3][1],
			 TransformMatrix[0][2] * Pos.x + TransformMatrix[1][2] * Pos.y + TransformMatrix[2][2] * Pos.z + TransformMatrix[3][2]
		 );

		 // Perform rotation on children node
		 for (auto pChildNode : pChildNodes)
		 {
			 auto pNode = pChildNode.lock();
			 pNode->RotateWithArbitraryAxis(Position, RotationAxis, Degree);
		 }
	}

	void Pawn::Scale(const glm::vec3& ScalingVector)
	{
		SetScalingFactor(GetScalingFactor() * ScalingVector);
	}

	glm::mat4 Pawn::GetModelMatrix() const
	{
		return GetRigidBodyModelMatrix();
	}
}