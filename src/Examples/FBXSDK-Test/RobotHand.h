#ifndef ROBOTHAND_H
#define ROBOTHAND_H

#define ROBOTHAND_LEFT 0
#define ROBOTHAND_RIGHT 1

using HandType = int;

#include "..\..\Engine\EngineFacade\EngineFacade.h"
#include "GlobalVariable.h"

#include "Kinematics.h"

using namespace cxc;

enum class Finger : uint16_t {thumb,index,middle,ring,little};

class Robothand final : public Object3D  {

public:

	Robothand();
	~Robothand();

	Robothand(HandType side, const std::string &model_file);

	Robothand(const Robothand &) = delete;
	Robothand& operator=(const Robothand &) = delete;

	Robothand(const Robothand &&) = delete;
	Robothand& operator=(const Robothand &&) = delete;

public:

	// Load robot hand from constants
	bool LoadRobothand(HandType type,const std::string &model_file) noexcept;

	void InitOriginalDegrees() noexcept;

	void SetBaseDegrees(const std::vector<float> &bases) noexcept;

	float GetBaseDegrees(const std::string &joint_name) const noexcept;

	glm::vec3 GetOriginPos() const noexcept;

public: 
		
	bool RotateJoint(const std::string &JointName,float degree) noexcept;

	void ResetAllJointPose() noexcept;
	void ResetJointPose(const std::string &JointName) noexcept;

	void IntegralTranslation(float x, float y, float z) noexcept;
	void IntegralRotation(float sx, float sy, float sz, float dx, float dy, float dz, float degree) noexcept;

private:

	void UpdateRotationAxisPos(const std::string &JointName,const glm::vec3 &start,const glm::vec3 &direction,float degree) noexcept;
	void UpdateRotationAxisPos(const std::string &JointName,const glm::vec3 &TransVec) noexcept;

public:

	std::shared_ptr<RotationAxis> GetRotationAxisPtr(const std::string &JointName) const noexcept;

	std::map<std::string, std::shared_ptr<RotationAxis>> GetJointAxisMap() const noexcept;

	float GetJointDegree(const std::string &jointname) const noexcept;

	// Kinematics interface
public:

	void MovingArm(const glm::mat4 &pose) noexcept;

	void MovingArm(const std::vector<float> &eular_space) noexcept;

	void MovingArm(GLfloat x, GLfloat y, GLfloat z, GLfloat x_angle, GLfloat y_angle, GLfloat z_angle) noexcept { MovingArm(std::vector<float>({x,y,z,x_angle,y_angle,z_angle})); };

	void MovingArmOffset(const glm::vec3 &pos_offset) noexcept;

	void MovingArmOffset(GLfloat x, GLfloat y, GLfloat z) noexcept { MovingArmOffset(glm::vec3({ x,y,z })); };
	
public:

	// Load motions from script
	void LoadMotionsFromScript(const std::string &scriptfile) noexcept;

public:
	// Coordinate transformation from left  to right coordinate system
	static glm::vec3 LeftToRightCoordinateTrans(const glm::vec3 &vec) noexcept;

	// Coordinate transformation from 3DS MAX to OpenGL, with XZ axis flipped
	static glm::vec3 CoordinateSysTransToOGL(const glm::vec3 &vec) noexcept;

	// Coordinate transformation from OpenGl to 3DS MAX, with XZ axis flipped
	static glm::vec3 CoordinateSysTransTo3DS(const glm::vec3 &vec) noexcept;

	// Coordinate transformation from left to right coodinate system
	static glm::vec3 RightToLeftCoordinateTrans(const glm::vec3 &vec) noexcept;

private:

	// ROBOTHAND_LEFT and ROBOTHAND_RIGHT
	HandType HType;

	// Origin of model, keep record of displacement
	glm::vec3 OriginPos;

	// Degrees of original pose
	// arm2-6|palm|thumb1-4|index1-4|middle1-4|ring1-4|little1-4
	// <Joint name, degree>
	std::map<std::string, float> m_JointDegrees;

	std::map<std::string, float> m_BaseDegrees;

	// <Joint name , axis pointer>
	std::map<std::string, std::shared_ptr<RotationAxis>> m_Joint_Axis;

	// <Joint name , angle constraint>
	std::unordered_map<std::string, std::pair<double, double>> m_JointConstraints;

	// <Finger knuckle name, length in Y-axis>
	std::unordered_map<std::string, uint16_t> m_FingerLength;

	// Script interface
	std::shared_ptr<Parser> script_parser;

private:
	
	// Check input angle 
	bool CheckAngleConstraint(const std::string &JointName, float degree) const noexcept;

	// build rotation axis
	void LoadRotationAxis(HandType type) noexcept;

	// Add root 
	void LoadRoot(HandType type) noexcept;

	// Add child 
	void LoadChild(HandType type) noexcept;

	// Add joint constraint
	void addJointConstraint() noexcept;

	// Add key points
	void addKeyPoints(HandType type) noexcept;

	// Add knuckle length
	void addKnuckleLength(HandType type) noexcept;

};

#endif // ROBOTHAND_H