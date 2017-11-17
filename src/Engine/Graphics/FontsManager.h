#include "..\General\DefineTypes.h"

#ifndef CXC_FONTSMANAGER_H
#define CXC_FONTSMANAGER_H

#include "Font.h"
#include "..\inl\Singleton.inl"
namespace cxc {

	class FontsManager : public Singleton<FontsManager>
	{
	public:
		friend Singleton<FontsManager>;

		explicit FontsManager();
		~FontsManager();

		FontsManager(const FontsManager &) = delete;
		FontsManager(const FontsManager &&) = delete;

		FontsManager& operator=(const FontsManager &) = delete;
		FontsManager& operator=(const FontsManager &&) = delete;

	public:

		const Font *GetFont(const std::string &FontName) const noexcept;
		void LoadFont(const std::string &FontTexPath);
		void RemoveAllFont() noexcept;
		GLboolean DeleteFont(const std::string &FontName) noexcept;

	private:

		// <FontName , Font object pointer>
		std::map <std::string, std::shared_ptr<Font>> m_FontsMap;

		// <FontName , texture path>
		std::map <std::string, std::string> m_FontsTexPath;
	};

}
#endif // CXC_FONTSMANAGER_H