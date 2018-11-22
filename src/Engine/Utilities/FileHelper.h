#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\inl\Singleton.inl"

#else

#include "../General/DefineTypes.h"
#include "../inl/Singleton.inl"

#endif // WIN32

#ifndef CXC_FILEHELPER_H
#define CXC_FILEHELPER_H

namespace cxc {

	class FileHelper final : public Singleton<FileHelper>
	{

	public:

		friend Singleton<FileHelper>;

		explicit FileHelper();
		~FileHelper();

	public:

		static GLboolean FileIsExists(const std::string &file_path);
		static std::string GetBaseDirectory(const std::string &filepath);
	};
}

#endif // CXC_FILEHELPER_H
