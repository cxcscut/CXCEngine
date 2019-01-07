
#ifndef CXC_DEFINETYPES_H
#define CXC_DEFINETYPES_H

#define CXC_SPRITE_SHADER_PROGRAM 1
#define CXC_FONT_SHADER_PROGRAM 2
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
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
#include <gl/glew.h>
#include <gl/glut.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// image loader library
#include "Image_loader/stb_image.h"

// template
#include "Utilities/MultiTree.inl"
#include "Utilities/EngineTemplate.inl"

// Engine core
#include "EngineCore.h"

namespace cxc {

	typedef GLint KeyType;

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

	enum class eBlendMode : uint16_t
	{
		Additive = 0,
		Normalized = 1,
		TotalOne = 2
	};
};

#endif // CXC_DEFINETYPES_H
