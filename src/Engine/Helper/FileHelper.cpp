#include "FileHelper.h"

namespace cxc {

	FileHelper::FileHelper() {}

	FileHelper::~FileHelper() {}

	GLboolean FileHelper::FileIsExists(const std::string &file_path)
	{
		std::ifstream ifs(file_path);

		if (!ifs)
			return GL_FALSE;
		else
			return GL_TRUE;
	}

	std::string FileHelper::GetBaseDirectory(const std::string &filepath) {
		if (filepath.find_last_of("/\\") != std::string::npos)
			return filepath.substr(0, filepath.find_last_of("/\\"));
		return "";
	}

}