
#ifndef CXC_DEFINETYPES_H
#define CXC_DEFINETYPES_H

#define CXC_SPRITE_SHADER_PROGRAM 1
#define CXC_FONT_SHADER_PROGRAM 2
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
#define CAMERA_FIXED (static_cast<cxc::CameraModeType>(cxc::CameraMode::CXC_CAMERA_FIXED))
#define CAMERA_FREE	 (static_cast<cxc::CameraModeType>(cxc::CameraMode::CXC_CAMERA_FREE))
#define PI 3.141592
#define USE_EBO

// C++ STL

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <functional>
#include <stack>
#include <deque>
#include <ctime>
#include <ratio>
#include <chrono>
#include <algorithm>
#include <queue>

// Multi-threading Synchronization
#include <thread>
#include <mutex>

// OpenGL

#ifdef WIN32

#include <gl/glew.h>
#include <gl/glut.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// image loader library
#include "..\Libraries\Image_loader\stb_image.h"

// template
#include "..\inl\MultiTree.inl"

#else

#include <GL/glew.h>
#include <GL/glut.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "../Libraries/Image_loader/stb_image.h"
#include "../inl//MultiTree.inl"

#endif

namespace cxc {

	// pre-declaration of Shape
	class Shape;

	typedef GLint ShaderType;
	typedef GLint KeyType;
	typedef GLuint CameraModeType;

	using RenderingThread =
		std::function<void(void)>;

	using ObjectTree = MultiTree<std::shared_ptr<Shape>>;
	using TokenTree = MultiTree<std::string>;

	typedef struct Color {
		GLfloat Red, Green, Blue, Alpha;

		Color() : Red(0.0f), Green(0.0f), Blue(0.0f), Alpha(0.0f)
		{}

		Color(float r, float g, float b, float alpha) {
			Red = r; Green = g, Blue = b, Alpha = alpha;
		}

		bool operator==(const Color &color) const {
			return this->Red == color.Red
				&& this->Green == color.Green
				&& this->Blue == color.Blue
				&& this->Alpha == color.Alpha;
		}
	} Color;

};

#endif // CXC_DEFINETYPES_H
