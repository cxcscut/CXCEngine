#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif // WIN32

#ifndef CXC_FONT_H
#define CXC_FONT_H

namespace cxc {

	class Font final
	{
	public:

		explicit Font();
		~Font();

		Font(const std::string &TexPath);

		Font(const Font &) = delete;
		Font(const Font &&) = delete;

		Font& operator=(const Font &) = delete;
		Font& operator=(const Font &&) = delete;

	public:

		void InitText2D(const std::string &Tex_Path);
		void PrintText2D(const std::string &Text,
			GLint xpos, GLint ypos,
			GLuint size) const;
		void CleanText2D() const;

		void LoadFontTexFromFile(const std::string &TexPath);
		void AccquireFontResources();
		void ReleaseFontResources();

		GLuint GetFontTexID() const noexcept;

		GLint GetFontProgramID() const noexcept;

	private:

		GLint FontProgram;
		GLuint FontTexID;

	};

}
#endif // CXC_FONT_H
