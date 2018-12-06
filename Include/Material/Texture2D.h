#include "General/DefineTypes.h"

#ifndef CXC_TEXTURE2D_H
#define CXC_TEXTURE2D_H

namespace cxc {

	using TextureAttrib = struct TextureAttrib {
		GLint Width;
		GLint Height;
		GLint Compoent;
		GLuint TextureID;
	};

	class Texture2D final
	{

	public:

		explicit Texture2D();
		~Texture2D();

		Texture2D(const Texture2D &texture) = delete;
		Texture2D& operator= (const Texture2D &texture) = delete;

		Texture2D(const Texture2D &&r_ref) = delete;
		Texture2D& operator== (const Texture2D &&r_ref) = delete;

		Texture2D(const std::string &tex_path);

	public:

		GLuint GetTextureID() const noexcept;
		GLint GetWidth() const noexcept;
		GLint GetHeight() const noexcept;
		GLint GetCompoent() const noexcept;

	// Texture loading
	public:

		GLboolean LoadTextureFromFile(const std::string &file_path) ;

		void releaseTexture() noexcept;
	private:

		// File name of the texture
		std::string FileName;

		// Texture ID
		GLuint TextureID;

		// Dimension of the texture ranging from 1 to 4
		GLint Compoent;

		// Width and height of the texture
		GLint Width, Height;

	};

}
#endif // CXC_TEXTURE_H
