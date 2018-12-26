#ifndef CXC_PAWN_H
#define CXC_PAWN_H

#include "Scene/Object3D.h"
#include "Physics/RigidBody3D.h"

namespace cxc
{
	/* Pawn is an object that can simulate physics */
	class Pawn : public Object3D , public RigidBody3D
	{
	public :
		Pawn();
		Pawn(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals);
		Pawn(std::vector<glm::vec3>& Vertices,
			std::vector<glm::vec3>& Normals,
			std::vector<glm::vec2>& UVs,
			std::vector<uint32_t>& Indices);

		Pawn(const std::string &name);
		Pawn(const std::string &name, const std::string &filename, const std::string &_tag = "", GLboolean _enable = GL_TRUE);

		virtual ~Pawn();

	public:

		virtual void Translate(const glm::vec3 &TranslationVector) override;
		virtual void RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree) override;
		virtual void RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree) override;
		virtual void RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree) override;
		virtual void Scale(const glm::vec3& ScalingVector) override;

		virtual glm::mat4 GetModelMatrix() const override;

		virtual void Tick(float DeltaSeconds) override;

		virtual void PhysicalTick(float DeltaSeconds);

	private:

		void InitializeRigidBody();
	};
}

#endif // CXC_PAWN_H