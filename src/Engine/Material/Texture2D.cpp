#include "Texture2D.h"

#ifdef WIN32

#include "..\Utilities\FileHelper.h"

#else

#include "../Utilities/FileHelper.h"

#endif // WIN32

namespace cxc {

	Texture2D::Texture2D() :
		TextureID(0),
		Width(0), Height(0),
		Compoent(0)
	{

	}

	Texture2D::~Texture2D()
	{
		if (TextureID) {
			glDeleteTextures(1, &TextureID);
			TextureID = 0;
		}
	}

	Texture2D::Texture2D(const std::string &tex_path)
		: Texture2D()
	{
		LoadTextureFromFile(tex_path);
	}

	GLint Texture2D::GetCompoent() const noexcept
	{
		return Compoent;
	}

	GLint Texture2D::GetWidth() const noexcept
	{
		return Width;
	}

	GLint Texture2D::GetHeight() const noexcept
	{
		return Height;
	}

	GLuint Texture2D::GetTextureID() const noexcept
	{
		return TextureID;
	}

	void Texture2D::releaseTexture() noexcept
	{
		if (TextureID)
			glDeleteTextures(1,&TextureID);
	}

	GLboolean Texture2D::LoadTextureFromFile(const std::string &file_path)
	{
		unsigned char *image;

		auto FileHelper = FileHelper::GetInstance();

		if (!FileHelper->FileIsExists(file_path))
			return GL_FALSE;

		image = stbi_load(file_path.c_str(), &Width, &Height, &Compoent, STBI_default);

		if (!image)
			return GL_FALSE;

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (Compoent == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}
		else if(Compoent == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(image);

		FileName = file_path;

		return GL_TRUE;
	}
}
