#include "Camera.h"

namespace cxc {

	GLdouble Camera::LastTime = glfwGetTime();

	Camera::Camera() :
		Projection(glm::mat4(1.0f)), View(glm::mat4(1.0f)),
		HorizontalAngle(3.14f), VerticalAngle(0.0f),
		InitialFov(45.0f),
		Speed(3.0f), MouseSpeed(0.005f),
		DeltaTime(0.0f), CurrentTime(0.0f),
		Position(glm::vec3(0, 3, 8)),
		origin(glm::vec3(0, 0, 0)), up_vector(0, 1, 0),
		m_CameraMode(static_cast<CameraModeType>(CameraMode::CXC_CAMERA_FIXED))
	{
		ComputeAngles();
	}

	Camera::~Camera()
	{

	}

	void Camera::ComputeAngles() noexcept
	{
		radius = glm::length(eye_pos - origin);
		if (radius > 1e-6)
			theta_y = acosf(eye_pos.y / radius);
		else
			theta_y = 0.0f;

		theta_xoz = atan2(eye_pos.x,eye_pos.z);

	}

	void Camera::ComputePosition() noexcept
	{
		eye_pos.y = radius * cos(theta_y);
		double r_xoz = radius * sin(theta_y);
		eye_pos.x = r_xoz * sin(theta_xoz);
		eye_pos.z = r_xoz * cos(theta_xoz);
	}

	void Camera::InitLastTime() noexcept
	{
		LastTime = glfwGetTime();
	}

	glm::mat4 Camera::GetProjectionMatrix() const noexcept
	{
		return Projection;
	}

	glm::mat4 Camera::GetViewMatrix() const noexcept
	{
		return View;
	}

	void Camera::SetAllMatrix(const glm::mat4 &ViewMatrix, 
							const glm::mat4 &ProjectionMatrix) noexcept
	{
		Projection = ProjectionMatrix;
		View = ViewMatrix;
	}

	GLfloat Camera::GetHrizontalAngle() const noexcept
	{
		return HorizontalAngle;
	}

	GLfloat Camera::GetVerticalAngle() const noexcept
	{
		return VerticalAngle;
	}

	void Camera::ComputeViewMatrix() noexcept
	{
		View = glm::lookAt(eye_pos,origin,up_vector);
	}

	GLfloat Camera::GetInitialFov() const noexcept
	{
		return InitialFov;
	}

	GLfloat Camera::GetSpeed() const noexcept
	{
		return Speed;
	}

	GLfloat Camera::GetMouseSpeed() const noexcept
	{
		return MouseSpeed;
	}

	void Camera::SetHorizontalAngle(GLfloat horizontalangle) noexcept
	{
		HorizontalAngle = horizontalangle;
	}

	void Camera::SetVerticalAngle(GLfloat verticalangle) noexcept
	{
		VerticalAngle = verticalangle;
	}

	void Camera::SetSpeed(GLfloat speed) noexcept
	{
		Speed = speed;
	}

	void Camera::SetMouseSpeed(GLfloat mousespeed) noexcept
	{
		MouseSpeed = mousespeed;
	}

	void Camera::UpdateCurrentTime() noexcept
	{
		CurrentTime = glfwGetTime();
	}

	void Camera::ComputeMatrices_Moving(GLFWwindow *window, GLint xpos, GLint ypos,GLint Height,GLint Width) noexcept
	{

		CurrentTime = glfwGetTime();

		DeltaTime = CurrentTime - LastTime;

		HorizontalAngle += MouseSpeed * static_cast<GLfloat>(Width / 2 - xpos);
		VerticalAngle += MouseSpeed * static_cast<GLfloat>(Height / 2 - ypos);

		glm::vec3 Direction(
			cos(VerticalAngle) * sin(HorizontalAngle),
			sin(VerticalAngle),
			cos(VerticalAngle)* cos(HorizontalAngle)
		);

		glm::vec3 Right(
			sin(HorizontalAngle - 3.14f / 2.0f),
			0,
			cos(HorizontalAngle - 3.14f / 2.0f)
		);

		glm::vec3 Up = glm::cross(Right, Direction); // glm::cross return the cross product of two input vectors

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			Position += Direction * DeltaTime * Speed;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			Position -= Direction * DeltaTime * Speed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			Position += Right * DeltaTime * Speed;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			Position -= Right * DeltaTime * Speed;
		}

		GLfloat FoV = InitialFov;

		Projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);


		View = glm::lookAt(
			Position,
			Position + Direction,
			Up
		);

		LastTime = CurrentTime;
	}

	void Camera::BindCameraUniforms(GLuint ProgramID) noexcept
	{
		BindViewMatrix(ProgramID);
		BindProjectionMatrix(ProgramID);
	}

	void Camera::BindViewMatrix(GLuint ProgramID) const noexcept 
	{
		GLuint ViewMatrixLoc = glGetUniformLocation(ProgramID, "V");
		glUniformMatrix4fv(ViewMatrixLoc, 1, GL_FALSE, &View[0][0]);
	}

	void Camera::BindProjectionMatrix(GLuint ProgramID) const noexcept
	{
		GLuint ProjectionMatrixLoc = glGetUniformLocation(ProgramID, "P");
		glUniformMatrix4fv(ProjectionMatrixLoc, 1, GL_FALSE, &Projection[0][0]);
	}
}
