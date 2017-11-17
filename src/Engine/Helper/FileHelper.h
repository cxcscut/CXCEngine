#include "..\General\DefineTypes.h"

#ifndef CXC_FILEHELPER_H
#define CXC_FILEHELPER_H

#include "..\inl\Singleton.inl"

namespace cxc {

	class FileHelper final : public Singleton<FileHelper>
	{

	public:

		friend Singleton<FileHelper>;

		explicit FileHelper();
		~FileHelper();

	public:

		GLboolean FileIsExists(const std::string &file_path);
		std::string GetBaseDirectory(const std::string &filepath);
	};
}

#endif // CXC_FILEHELPER_H
