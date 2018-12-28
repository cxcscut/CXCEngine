#include "General/DefineTypes.h"
#include "Utilities/Singleton.inl"

#ifndef CXC_FILEHELPER_H
#define CXC_FILEHELPER_H

namespace cxc {

	class CXC_ENGINECORE_API FileHelper final : public Singleton<FileHelper>
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
