#ifndef CXC_SINGLETON_INL
#define CXC_SINGLETON_INL

#include <memory>
#include "General/EngineCore.h"

namespace cxc {

	// Singleton.inl is the implementation of the singleton design pattern for managing the 
	// allocation and releasing of singleton instance

	// WARNING : The instance of the singleton class can not be obtained in the contruction function
	// which will cause stack overflowing.
	
	template<class SingletonClass>
	class CXC_ENGINECORE_API Singleton
	{
	public:
		template<typename... Args>
		static inline std::shared_ptr<SingletonClass> GetInstance(Args&&... args) noexcept;
		static inline void DeleteSingleton() noexcept;

	protected:
		Singleton() {}
		virtual ~Singleton() {}

		Singleton(Singleton<SingletonClass> & InObject);
		Singleton(Singleton<SingletonClass> && InObject);

		Singleton& operator=(Singleton<SingletonClass> & InObject) {
			pInstance.reset(InObject.pInstance.get());
			InObject.DeleteSingleton();
		}
		Singleton& operator=(Singleton<SingletonClass> && InObject) {
			pInstance.reset(InObject.pInstance.get());
			InObject.DeleteSingleton();
		}

	private:

		static std::shared_ptr<SingletonClass> pInstance;
	};

	template<class SingletonClass>
	std::shared_ptr<SingletonClass> Singleton<SingletonClass>::pInstance;

	template<class SingletonClass>
	template<typename... Args>
	std::shared_ptr<SingletonClass> Singleton<SingletonClass>::
		GetInstance(Args&&... args) noexcept {
		if (pInstance.get() == nullptr) {
			pInstance = std::make_shared<SingletonClass>(std::forward<Args>(args)...);
		}
		return pInstance;
	}

	template<class SingletonClass>
	inline
		void Singleton<SingletonClass>::
		DeleteSingleton() noexcept {
		pInstance.reset();
	}

	template<class SingletonClass>
	Singleton<SingletonClass>::Singleton(Singleton<SingletonClass> & InObject) {
		pInstance.reset(InObject.pInstance.get());
		InObject.DeleteSingleton();
	}

	template<class SingletonClass>
	Singleton<SingletonClass>::Singleton(Singleton<SingletonClass> && InObject) {
		pInstance.reset(InObject.pInstance.get());
		InObject.DeleteSingleton();
	}
}

#endif // CXC_SINGLETON_INL
