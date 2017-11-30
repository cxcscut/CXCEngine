#include "Kinematics.h"

using namespace std;

glm::mat4 KineSolver::ForwardKinematics(const std::vector<float> &joint_degrees, GLuint start_joint, GLuint end_joint)
{
	glm::mat4 pose(1.0f);
	float cos_theta, sin_theta, cos_alpha, sin_alpha;

	if (joint_degrees.size() != ARM_JOINT_NUM) return pose;

	assert(a.size() == ARM_JOINT_NUM);
	assert(d.size() == ARM_JOINT_NUM);
	assert(alpha.size() == ARM_JOINT_NUM);

	for (auto i = start_joint; i < end_joint; i++)
	{
		cos_theta = cosf(glm::radians(joint_degrees[i]));
		sin_theta = sinf(glm::radians(joint_degrees[i]));
		cos_alpha = cosf(glm::radians(alpha[i]));
		sin_alpha = sinf(glm::radians(alpha[i]));


		glm::mat4 Ai = glm::transpose(
			glm::mat4({ cos_theta, -sin_theta * cos_alpha,sin_theta * sin_alpha, a[i] * cos_theta,
				sin_theta, cos_theta * cos_alpha, -cos_theta * sin_alpha,a[i] * sin_theta,
				0,		   sin_alpha,             cos_alpha,             d[i],
				0,         0,                     0,                     1 })
		);


		pose = pose * Ai;
	}

	return pose;
}

float KineSolver::matrixMeanSquareError(const glm::mat4 &mat1, const glm::mat4 &mat2)
{
	float sum = 0.0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			sum += (mat1[i][j] - mat2[i][j])*(mat1[i][j] - mat2[i][j]);

	return sum / 16;
}

std::vector<std::vector<float>> KineSolver::_InvKine(const glm::mat4 &pose)
{
	vector<vector<float>> solutions;
	auto nx = pose[0][0], ny = pose[0][1], nz = pose[0][2];
	auto ox = pose[1][0], oy = pose[1][1], oz = pose[1][2];
	// theta1
	glm::vec3 N06k0 = { pose[2][0],pose[2][1],pose[2][2] };
	auto p46k0 = d[5] * N06k0;
	glm::vec3 p06k0 = { pose[3][0],pose[3][1],pose[3][2] };
	auto p04k0 = p06k0 - p46k0;
	auto theta1_1 = atan2(p04k0.y, p04k0.x);
	auto theta1_2 = theta1_1 + glm::radians(180.0f);


	//*******************************************************************************************************

	// theta 3 using theta1_1
	glm::vec3 p01k0 = { a[0] * cosf(theta1_1),a[0] * sinf(theta1_1),d[0] };
	auto p14k0 = p04k0 - p01k0;
	auto l1 = sqrtf(a[2] * a[2] + d[3] * d[3]);
	auto dp = glm::length(p14k0);
	auto sp = (l1*l1 - a[1] * a[1] + dp*dp) / (2 * dp*l1);
	auto cp = (dp - (l1*l1 - a[1] * a[1] + dp*dp) / (2 * dp)) / a[1];
	float theta3_1, theta3_2, p;
	auto ap = atan2(-d[3], a[2]);
	if (fabs(sp) > 1.0f || fabs(cp) > 1.0f)
		p = 0.0f;
	else
		p = asinf(sp) + asinf(cp);
	
	theta3_1 = glm::radians(180.0f) - p - ap;
	theta3_2 = glm::radians(180.0f) + p - ap;


	// theta 3 using theta1_2
	p01k0 = glm::vec3({ a[0] * cosf(theta1_2),a[0] * sinf(theta1_2),d[0] });
	p14k0 = p04k0 - p01k0;
	dp = glm::length(p14k0);
	sp = (l1*l1 - a[1] * a[1] + dp*dp) / (2 * dp*l1);
	cp = (dp - (l1*l1 - a[1] * a[1] + dp*dp) / (2 * dp)) / a[1];
	ap = atan2(-d[3], a[2]);
	float theta3_3, theta3_4;
	if (fabs(sp) > 1.0f || fabs(cp) > 1.0f)
		p = 0.0f;
	else
		p = asinf(sp) + asinf(cp);

	theta3_3 = glm::radians(180.0f) - p - ap;
	theta3_4 = glm::radians(180.0f) + p - ap;

	//****************************************************************************************************************

	// theta 2 using theta1_1 

	glm::mat3 r01 = glm::transpose(glm::mat3({
		cosf(theta1_1),0,sinf(theta1_1),
		sinf(theta1_1),0,-cosf(theta1_1),
		0,1,0
	}));

	p01k0 = glm::vec3({ a[0] * cosf(theta1_1),a[0] * sinf(theta1_1),d[0] });
	p14k0 = p04k0 - p01k0;

	glm::vec3 p14k1 = glm::inverse(r01) * p14k0;

	dp = glm::length(p14k1);
	auto beta1 = atan2(p14k1.x, p14k1.y);
	auto beta_sp = (a[1] * a[1] - dp*dp + l1*l1) / (2 * l1*a[1]);
	auto beta_cp = (l1 - (a[1] * a[1] - dp*dp + l1*l1) / (2 * l1)) / (dp);
	float beta2;
	if (fabs(beta_sp) > 1.0f || fabs(beta_cp) > 1.0f)
		beta2 = 0.0f;
	else
		beta2 = asinf(beta_sp) + asinf(beta_cp);

	auto theta2_1 = glm::radians(90.0f) - (fabsf(beta1) + beta2);
	auto theta2_2 = glm::radians(90.0f) + (fabsf(beta1) - beta2);


	// theta 2 using theta1_2
	r01 = glm::transpose(glm::mat3({
		cosf(theta1_2),0,sinf(theta1_2),
		sinf(theta1_2),0,-cosf(theta1_2),
		0,1,0
	}));

	p01k0 = glm::vec3({ a[0] * cosf(theta1_2),a[0] * sinf(theta1_2),d[0] });
	p14k0 = p04k0 - p01k0;
	p14k1 = glm::inverse(r01) * p14k0;
	dp = glm::length(p14k1);
	beta1 = atan2(p14k1.x, p14k1.y);
	beta_sp = (a[1] * a[1] - dp*dp + l1*l1) / (2 * l1*a[1]);
	beta_cp = (l1 - (a[1] * a[1] - dp*dp + l1*l1) / (2 * l1)) / (dp);
	if (fabs(beta_sp) > 1.0f || fabs(beta_cp) > 1.0f)
		beta2 = 0.0f;
	else
		beta2 = asinf(beta_sp) + asinf(beta_cp);
	
	auto theta2_3 = glm::radians(90.0f) - (fabsf(beta1) + beta2);
	auto theta2_4 = glm::radians(90.0f) + (fabsf(beta1) - beta2);

	//****************************************************************************************************************

	// theta 5 using theta1_1 & theta2_1 & theta3_1
	auto c1 = cosf(theta1_1);
	auto s23 = sinf(theta2_1 + theta3_1);
	auto s1 = sinf(theta1_1);
	auto c23 = cosf(theta2_1 + theta3_1);
	glm::vec3 N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_1 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_1 & theta2_1 & theta3_2
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_1 + theta3_2);
	s1 = sinf(theta1_1);
	c23 = cosf(theta2_1 + theta3_2);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_2 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_1 & theta2_2 & theta3_1
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_1);
	s1 = sinf(theta1_1);
	c23 = cosf(theta2_2 + theta3_1);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_3 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_1 & theta2_2 & theta3_2
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_2);
	s1 = sinf(theta1_1);
	c23 = cosf(theta2_2 + theta3_2);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_4 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_2 & theta2_3 & theta3_3
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_3);
	s1 = sinf(theta1_2);
	c23 = cosf(theta2_3 + theta3_3);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_5 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_2 & theta2_3 & theta3_4
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_4);
	s1 = sinf(theta1_2);
	c23 = cosf(theta2_3 + theta3_4);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_6 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_2 & theta2_4 & theta3_3
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_3);
	s1 = sinf(theta1_2);
	c23 = cosf(theta2_4 + theta3_3);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_7 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	// theta 5 using theta1_2 & theta2_4 & theta3_4
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_4);
	s1 = sinf(theta1_2);
	c23 = cosf(theta2_4 + theta3_4);
	N03k0 = { c1*s23,s1*s23,-c23 };

	auto theta5_8 = glm::radians(180.0f) - acosf(glm::dot(N03k0, N06k0));

	//****************************************************************************************************************

	// theta 4 using theta 5_1 & theta1_1 & theta2_1 & theta3_1
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_1 + theta3_1);
	c23 = cosf(theta2_1 + theta3_1);
	auto theta4_1 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_2 & theta1_1 & theta2_1 & theta3_2
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_1 + theta3_2);
	c23 = cosf(theta2_1 + theta3_2);
	auto theta4_2 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_3 & theta1_1 & theta2_2 & theta3_1
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_1);
	c23 = cosf(theta2_2 + theta3_1);
	auto theta4_3 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_4 & theta1_1 & theta2_2 & theta3_2
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_2);
	c23 = cosf(theta2_2 + theta3_2);
	auto theta4_4 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_5 & theta1_2 & theta2_3 & theta3_3
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_3);
	c23 = cosf(theta2_3 + theta3_3);
	auto theta4_5 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_6 & theta1_2 & theta2_3 & theta3_4
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_4);
	c23 = cosf(theta2_3 + theta3_4);
	auto theta4_6 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_7 & theta1_2 & theta2_4 & theta3_3
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_3);
	c23 = cosf(theta2_4 + theta3_3);
	auto theta4_7 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	// theta 4 using theta 5_8 & theta1_2 & theta2_4 & theta3_4
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_4);
	c23 = cosf(theta2_4 + theta3_4);
	auto theta4_8 = atan2(c1*N06k0.y - s1*N06k0.x, -c1*c23*N06k0.x - s1*c23*N06k0.y - s23*N06k0.z);

	//****************************************************************************************************************

	// theta 6 using theta 4_1 & theta 5_1 & theta1_1 & theta2_1 & theta3_1
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_1 + theta3_1);
	c23 = cosf(theta2_1 + theta3_1);
	auto theta6_1 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_1,theta2_1, theta3_1,theta4_1,theta5_1,theta6_1 }));

	// theta 6 using theta 4_2 & theta 5_2 & theta1_1 & theta2_1 & theta3_2
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_1 + theta3_2);
	c23 = cosf(theta2_1 + theta3_2);
	auto theta6_2 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_1,theta2_1, theta3_2,theta4_2,theta5_2,theta6_2 }));

	// theta 6 using theta 4_3 & theta 5_3 & theta1_1 & theta2_2 & theta3_1
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_1);
	c23 = cosf(theta2_2 + theta3_1);
	auto theta6_3 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_1,theta2_2, theta3_1,theta4_3,theta5_3,theta6_3 }));

	// theta 6 using theta 4_4 & theta 5_4 & theta1_1 & theta2_2 & theta3_2
	s1 = sinf(theta1_1);
	c1 = cosf(theta1_1);
	s23 = sinf(theta2_2 + theta3_2);
	c23 = cosf(theta2_2 + theta3_2);
	auto theta6_4 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_1,theta2_2, theta3_2,theta4_4,theta5_4,theta6_4 }));

	// theta 6 using theta 4_5 & theta 5_5 & theta1_2 & theta2_3 & theta3_3
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_3);
	c23 = cosf(theta2_3 + theta3_3);
	auto theta6_5 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_2,theta2_3, theta3_3,theta4_5,theta5_5,theta6_5 }));

	// theta 6 using theta 4_6 & theta 5_6 & theta1_2 & theta2_3 & theta3_4
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_3 + theta3_4);
	c23 = cosf(theta2_3 + theta3_4);
	auto theta6_6 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_2,theta2_3, theta3_4,theta4_6,theta5_6,theta6_6 }));

	// theta 6 using theta 4_7 & theta 5_7 & theta1_2 & theta2_4 & theta3_3
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_3);
	c23 = cosf(theta2_4 + theta3_3);
	auto theta6_7 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_2,theta2_4, theta3_3,theta4_7,theta5_7,theta6_7 }));

	// theta 6 using theta 4_8 & theta 5_8 & theta1_2 & theta2_4 & theta3_4
	s1 = sinf(theta1_2);
	c1 = cosf(theta1_2);
	s23 = sinf(theta2_4 + theta3_4);
	c23 = cosf(theta2_4 + theta3_4);
	auto theta6_8 = atan2(c1*s23*ox + s1*s23*oy - c23*oz, -c1*s23*nx - s1*s23*ny + c23*nz);

	solutions.emplace_back(vector<float>({ theta1_2,theta2_4, theta3_4,theta4_8,theta5_8,theta6_8 }));

	return solutions;
}

glm::mat4 KineSolver::EularSpaceToOrientation(const vector<float> &eular_space)
{
	if (eular_space.size() < 6) return glm::mat4(1.0f);

	auto px = eular_space[0], py = eular_space[1], pz = eular_space[2];
	auto sina = sinf(eular_space[5]), cosa = cosf(eular_space[5]);
	auto sinp = sinf(eular_space[3]), cosp = cosf(eular_space[3]);
	auto sing = sinf(eular_space[4]), cosg = cosf(eular_space[4]);
	return glm::transpose(glm::mat4({
		cosp*sing,cosp*sing*sina - sinp*cosa,cosp*sing*cosa + sinp*sina,px,
		sinp*cosg,sinp*sing*sina + cosp*cosa,sinp*sing*cosa - cosp*sina,py,
		-sing,cosg*sina,cosg*cosa,pz,
		0,0,0,1
	}));
}

GLboolean KineSolver::getOptimalSolution(std::vector<std::vector<float>> &solutions, const glm::mat4 &target_pose,float error, std::vector<float> &sol)
{
	vector<float> errors;

	for (auto &solution : solutions)
		for (auto &angle : solution)
			angle = glm::degrees(angle);

	for (auto solution : solutions)
	{
		auto _pose = ForwardKinematics(solution, 0, 6);
		errors.emplace_back(matrixMeanSquareError(target_pose, _pose));
	}

	auto pmin = min_element(begin(errors), end(errors));

	if (*pmin <= error) {
		sol = solutions[distance(begin(errors), pmin)];
		return GL_TRUE;
	}
	else {
		return GL_FALSE;
	}
}

GLboolean KineSolver::getOptimalSolution(std::vector<std::vector<float>> &solutions, const std::vector<float> &target_pose, float error, std::vector<float> &sol)
{
	vector<float> errors;

	for (auto &solution : solutions)
		for (auto &angle : solution)
			angle = glm::degrees(angle);

	for (auto solution : solutions)
	{
		auto _pose = ForwardKinematics(solution, 0, 6);
		errors.emplace_back(matrixMeanSquareError(EularSpaceToOrientation(target_pose), _pose));
	}

	auto pmin = min_element(begin(errors), end(errors));

	if (*pmin <= error) {
		sol = solutions[distance(begin(errors), pmin)];
		return GL_TRUE;
	}
	else {
		return GL_FALSE;
	}
}

std::vector<std::vector<float>> KineSolver::InverseKinematics(const glm::mat4 &pose)
{
	return _InvKine(pose);
}

std::vector<std::vector<float>> KineSolver::InverseKinematics(const std::vector<float> &pose)
{
	return _InvKine(EularSpaceToOrientation(pose));
}