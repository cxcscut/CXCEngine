#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Utilities\Singleton.inl"

#else

#include "../General/DefineTypes.h"
#include "../Utilities/Singleton.inl"

#endif

#ifndef CXC_SHADERMANAGER_H
#define CXC_SHADERMANAGER_H

#include "Shader.h"

namespace cxc
{
	class ShaderManager : public Singleton<ShaderManager>
	{
		friend class Singleton<ShaderManager>;

	public:

		ShaderManager();
		~ShaderManager();

	public:

		std::shared_ptr<Shader> GetShader(const std::string& ShaderName);
		void AddShader(std::shared_ptr<Shader> pShader);
		std::shared_ptr<Shader> FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName);
	private:

		std::unordered_map<std::string, std::shared_ptr<Shader>> pShadersMap;

	};

}

#endif // CXC_SHADERMANAGER_H