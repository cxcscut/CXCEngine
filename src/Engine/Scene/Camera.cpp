#include "Camera.h"

namespace cxc {

	Camera::Camera() :
		Projection(glm::mat4(1.0f)), View(glm::mat4(1.0f)),
		HorizontalAngle(3.14f), VerticalAngle(0.0f),
		InitialFov(45.0f),
		Speed(3.0f), MouseSpeed(0.005f),
		DeltaTime(0.0f), CurrentTime(0.0f),
		Position(glm::vec3(0, 3, 8)),
		CameraOrigin(glm::vec3(0, 0, 0)), UpVector(0, 0, 1)
	{
		ComputeAngles();
	}

	Camera::~Camera()
	{

	}

	void Camera::ComputeAngles() noexcept
	{
		Radius = glm::length(EyePosition - CameraOrigin);

		double RadiusXOY = sqrtf((EyePosition.x - CameraOrigin.x) * (EyePosition.x - CameraOrigin.x) + 
			(EyePosition.y - CameraOrigin.y) * (EyePosition.y - CameraOrigin.y));
		ThetaToXOY = atan2f(EyePosition.z, RadiusXOY);

		// ThetaToXOY must be clamped to (-PI / 2, PI / 2)
		ThetaToXOY = std::fmin(PI / 2 - 0.1f, ThetaToXOY);
		ThetaToXOY = std::fmax(-PI / 2 + 0.1f, ThetaToXOY);

		ThetaXOY = atan2f(EyePosition.y ,EyePosition.x);
	}

	void Camera::ComputePosition() noexcept
	{
		EyePosition.z = Radius * sinf(ThetaToXOY);
		double RadiusXOY = Radius * cosf(ThetaToXOY);
		EyePosition.x = RadiusXOY * cosf(ThetaXOY);
		EyePosition.y = RadiusXOY * sinf(ThetaXOY);
	}

	glm::mat4 Camera::GetProjectionMatrix() const noexcept
	{
		return Projection;
	}

	glm::vec3 Camera::GetCameraRightVector() const noexcept
	{
		// The right vector of the camera equals to the cross product of up vector and forward vector
		return glm::normalize(glm::cross(UpVector, GetCameraForwardVector()));
	}

	glm::vec3 Camera::GetCameraForwardVector() const noexcept
	{
		return glm::normalize(CameraOrigin - EyePosition);
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
		View = glm::lookAt(EyePosition, CameraOrigin, UpVector);
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

	bool Camera::isVertexInFrustum(const glm::vec3 &v) const noexcept
	{
		glm::vec4 _v = glm::vec4(v.x,v.y,v.z,1);

		_v = Projection * View * _v;
		auto __v = glm::vec3(_v.x/_v.w,_v.y/_v.w,_v.z/_v.w);

		return (__v.x - 1.0f <= 1e-7 && __v.x + 1.0f >= 1e-7) &&
			(__v.y - 1.0f <= 1e-7 && __v.y + 1.0f >= 1e-7) &&
			(__v.z - 1.0f <= 1e-7 && __v.z + 1.0f >= 1e-7);
	}

	bool Camera::isRectInFrustum(const glm::vec3 &max, const glm::vec3 &min) const noexcept
	{
		glm::vec4 _max = glm::vec4(max.x, max.y, max.z, 1);
		glm::vec4 _min = glm::vec4(min.x, min.y, min.z, 1);

		_max = Projection * View * _max;
		_min = Projection * View * _min;

		glm::vec3 nmax = glm::vec3(_max.x / _max.w, _max.y / _max.w, _max.z / _max.w);
		glm::vec3 nmin = glm::vec3(_min.x / _min.w, _min.y / _min.w, _min.z / _min.w);
		
		bool ret = false;
		ret = ret || isVertexInFrustum(max) || isVertexInFrustum(min) ||
			isVertexInFrustum(glm::vec3(min.x, max.y, min.z)) ||
			isVertexInFrustum(glm::vec3(max.x, min.y, min.z)) ||
			isVertexInFrustum(glm::vec3(max.x, max.y, min.z)) ||
			isVertexInFrustum(glm::vec3(min.x, min.y, max.z)) ||
			isVertexInFrustum(glm::vec3(min.x, max.y, max.z)) ||
			isVertexInFrustum(glm::vec3(max.x, min.y, max.z));

		return ret || (
			(nmax.x >= 1.0f && nmin.x <= -1.0f) || (nmax.x <= -1.0f && nmin.x >= 1.0f) ||
			(nmax.y >= 1.0f && nmin.y <= -1.0f) || (nmax.y <= -1.0f && nmin.y >= 1.0f) &&
			((nmin.x >= -1.0f && nmin.x <= 1.0f) || (nmin.x <= -1.0f && -1.0f <= nmax.x))
			);
	}
}
