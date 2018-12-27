#ifndef CXC_CACTOR_H
#define CXC_CACTOR_H

#include "CObject.h"
#include "General/DefineTypes.h"

namespace cxc
{
	/* CActor is the class of a plceable object in the world, which can be moved around or rotated and scaled */
	class CActor : public CObject
	{
	public:
		CActor();
		CActor(const std::string& Name, const std::string& Tag = "");
		virtual ~CActor();

	public:

		virtual void Translate(const glm::vec3 &TranslationVector);
		virtual void RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree);
		virtual void RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree);
		virtual void RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree);
		virtual void Scale(const glm::vec3& ScalingVector);

		virtual glm::mat4 GetModelMatrix() const;

	};
}

#endif // CXC_CACTOR_H