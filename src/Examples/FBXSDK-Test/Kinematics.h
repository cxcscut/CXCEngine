#ifndef KINEMATICS_H
#define KINEMATICS_H

#define ARM_JOINT_NUM 6U

#include "../../Engine/General/DefineTypes.h"

// D-H parameters 

// d
static const std::vector<float> d = { 380.948f,0,0,-560.181f,0,94.77f };

// a
static const std::vector<float> a = {
	-220.0f,522.331f,80.0f,0,0,0 };

// alpha
static const std::vector<float> alpha = {
	90,0,90,-90,-90,0 };


class KineSolver {

public:

	KineSolver();
	~KineSolver();

	KineSolver(const KineSolver &) = delete;
	KineSolver(const KineSolver &&) = delete;

public:

	// Forward kinematics
	static glm::mat4 ForwardKinematics(const std::vector<float> &joint_degrees, GLuint start_joint = 0U, GLuint end_joint = ARM_JOINT_NUM);

	// Inverse kinematics interface adapter
	static std::vector<std::vector<float>> InverseKinematics(const glm::mat4 &pose);

	static std::vector<std::vector<float>> InverseKinematics(const std::vector<float> &pose);
	

	// Select optimal solution
	static GLboolean getOptimalSolution(std::vector<std::vector<float>> &solutions, const glm::mat4 &target_pose, float error, std::vector<float> &so);

	static GLboolean getOptimalSolution(std::vector<std::vector<float>> &solutions, const std::vector<float> &target_pose, float error, std::vector<float> &so);

	// Conversion from Eular space to orientation matrix
	// <px,py,pz,p,g,a>
	static glm::mat4 EularSpaceToOrientation(const std::vector<float> &eular_space);

private:

	// Compute mean square errors
	static float matrixMeanSquareError(const glm::mat4 &mat1, const glm::mat4 &mat2);

	// Inverse Kinematics
	static std::vector<std::vector<float>> _InvKine(const glm::mat4 &pose);

};

#endif // KINEMATICS_H
