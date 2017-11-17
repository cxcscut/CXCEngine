
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
#include <map>
#include <functional>
#include <stack>
#include <deque>
#include <ctime>
#include <ratio>
#include <chrono>
#include <algorithm>

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
#include "..\Libraries\Image_loader\stb_image.h"

// template
#include "..\inl\MultiTree.inl"

namespace cxc {

	// pre-declaration of Shape
	class Shape;

	typedef GLint ShaderType;
	typedef GLint KeyType;
	typedef GLuint CameraModeType;

	using RenderingThread =
		std::function<void(void)>;

	using KeyboardHandler =
		std::function<void(void)>;

	using ScrollHandler =
		std::function<void(void)>;

	typedef KeyboardHandler MouseHandler;

	using ObjectTree = MultiTree<std::shared_ptr<Shape>>;
	using TokenTree = MultiTree<std::string>;

	// position,texcoord,normal,color
	using BufferIDs = struct BufferIDs {
		GLuint VBO_ID[4]; 
		GLuint VAO_ID;
		GLuint EBO_ID;

		BufferIDs()
			: VAO_ID(0), EBO_ID(0)
		{
			VBO_ID[0] = VBO_ID[1] = VBO_ID[2] = VBO_ID[3] = 0;
		}
	};

	using VertexIndexPacket = struct VertexIndexPacket {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;

		VertexIndexPacket()
			:position(glm::vec3(0.0f, 0.0f, 0.0f)),
			normal(glm::vec3(0.0f,0.0f,0.0f)),
			texcoords(glm::vec2(0.0f,0.0f))
		{}

		VertexIndexPacket(const glm::vec3 &pos,const glm::vec3 &_normal,const glm::vec2 &uv)
		{
			position = pos;
			normal = _normal;
			texcoords = uv;
		}

		// For customized type, overloaded operator < is needed for std::map
		bool operator<(const VertexIndexPacket &that) const {
			return memcmp((void*)this, (void*)&that, sizeof(VertexIndexPacket)) > 0;
		};
	};

	using TextureAttrib = struct TextureAttrib {
		GLint Width;
		GLint Height;
		GLint Compoent;
		GLuint TextureID;
	};

	enum class CameraMode : GLuint {
		CXC_CAMERA_FIXED = 0,
		CXC_CAMERA_FREE = 1
	};

	enum class Location : GLuint {
		VERTEX_LOCATION = 0,
		TEXTURE_LOCATION = 1,
		NORMAL_LOCATION = 2,
		COLOR_LOCATION = 3,
		NUM_OF_LOCATION = 4
	};


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

	typedef struct ProgramStruct {
		GLint ProgramID;
		std::vector<GLint> VertexShader;
		std::vector<GLint> FragmentShader;

		ProgramStruct& operator=(const ProgramStruct & rhs) {
			this->ProgramID = rhs.ProgramID;
			this->VertexShader = rhs.VertexShader;
			this->FragmentShader = rhs.FragmentShader;
			return *this;
		}
		void clear() {
			ProgramID = 0;
			VertexShader.clear();
			FragmentShader.clear();
		}
		void ReConstruct(GLint ID, GLint VertexShaderID, GLint FragmentShaderID) {
			this->clear();
			ProgramID = ID;
			VertexShader.emplace_back(VertexShaderID);
			FragmentShader.emplace_back(FragmentShaderID);
		}
	} ProgramStruct;

};

#endif // CXC_DEFINETYPES_H
