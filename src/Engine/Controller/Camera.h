
#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\inl\Singleton.inl"

#else

#include "../General/DefineTypes.h"
#include "../inl/Singleton.inl"

#endif

#ifndef CXC_CAMERA_H
#define CXC_CAMERA_H

namespace cxc {

	enum class CameraMode : GLuint {
		CXC_CAMERA_FIXED = 0,
		CXC_CAMERA_FREE = 1
	};

	class Camera final
	{

	public:

		friend class EngineFacade;
		friend class SceneManager;

		explicit Camera();
		~Camera();

		Camera(const Camera &) = delete;
		Camera& operator= (const Camera &) = delete;

		// Frustum culling
	public :
		
		bool isVertexInFrustum(const glm::vec3 &v) const noexcept;
		bool isRectInFrustum(const glm::vec3 &max, const glm::vec3 &min) const noexcept;

		// Private data access interface
	public:

		glm::mat4 GetProjectionMatrix() const noexcept;
		glm::mat4 GetViewMatrix() const noexcept;
		GLfloat GetHrizontalAngle() const noexcept;
		GLfloat GetVerticalAngle() const noexcept;
		GLfloat GetInitialFov() const noexcept;
		GLfloat GetSpeed() const noexcept;
		GLfloat GetMouseSpeed() const noexcept;

		void SetAllMatrix(const glm::mat4 &ViewMatrix, const glm::mat4 &ProjectionMatrix) noexcept;
		void SetHorizontalAngle(GLfloat horizontalangle) noexcept;
		void SetVerticalAngle(GLfloat verticalangle) noexcept;
		void SetSpeed(GLfloat speed) noexcept;
		void SetMouseSpeed(GLfloat mousespeed) noexcept;

	// Free camera computation
	public:

		void InitLastTime() noexcept;
		void UpdateCurrentTime() noexcept;
		void ComputeViewMatrix() noexcept;
		void ComputeMatrices_Moving(GLFWwindow *window, GLint xpos, GLint ypos,GLint Height,GLint Width) noexcept;

	// Data Transportation
	public:

		void BindCameraUniforms(GLuint ProgramID) noexcept;
		void BindViewMatrix(GLuint ProgramID) const noexcept;
		void BindProjectionMatrix(GLuint ProgramID) const noexcept;

	// Coorinates transformation
	public:

		void ComputeAngles() noexcept;
		void ComputePosition() noexcept;

	// Private data
	public:

		// Matrices of camera
		glm::mat4 Projection, View;

		// Free camera parameters
		GLfloat HorizontalAngle, VerticalAngle, InitialFov, Speed, MouseSpeed;
		GLfloat DeltaTime;
		GLdouble CurrentTime;
		GLdouble static LastTime;
		glm::vec3 Position;

		// Fixed camera parameters
		glm::vec3 eye_pos;
		glm::vec3 origin;
		glm::vec3 up_vector;

		double theta_xoz, theta_y, radius;

		CameraModeType m_CameraMode;

	};

}
#endif // CXC_CAMERA_H
