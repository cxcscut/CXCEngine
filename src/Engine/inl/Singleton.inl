#ifndef CXC_SINGLETON_INL
#define CXC_SINGLETON_INL

#include <memory>

namespace cxc {

	// Singleton.inl is the implementation of the singleton design pattern

	template<typename _Tx>
	class Singleton
	{
	public:
		template<typename...Args>
		static inline std::shared_ptr<_Tx> GetInstance(Args&&... args) noexcept;
		static inline void DeleteSingleton() noexcept;

	protected:
		Singleton() {}
		virtual ~Singleton() {}

		Singleton(Singleton<_Tx> & object);
		Singleton(Singleton<_Tx> && object);

		Singleton& operator=(Singleton<_Tx> & object) {
			_pSingletonObject.reset(object._pSingletonObject.get());
			object.DeleteSingleton();
		}
		Singleton& operator=(Singleton<_Tx> && object) {
			_pSingletonObject.reset(object._pSingletonObject.get());
			object.DeleteSingleton();
		}

	private:

		static std::shared_ptr<_Tx> _pSingletonObject;
	};

	template<typename _Tx>
	std::shared_ptr<_Tx> Singleton<_Tx>::_pSingletonObject;

	template<typename _Tx>
	template<typename... Args>
	std::shared_ptr<_Tx> Singleton<_Tx>::
		GetInstance(Args&&... args) noexcept {
		if (_pSingletonObject.get() == nullptr) {
			_pSingletonObject = std::make_shared<_Tx>(std::forward<Args>(args)...);
		}
		return _pSingletonObject;
	}

	template<typename _Tx>
	inline
		void Singleton<_Tx>::
		DeleteSingleton() noexcept {
		_pSingletonObject.reset();
	}

	template<typename _Tx>
	Singleton<_Tx>::Singleton(Singleton<_Tx> & object) {
		_pSingletonObject.reset(object._pSingletonObject.get());
		object.DeleteSingleton();
	}

	template<typename _Tx>
	Singleton<_Tx>::Singleton(Singleton<_Tx> && object) {
		_pSingletonObject.reset(object._pSingletonObject.get());
		object.DeleteSingleton();
	}
}

#endif // CXC_SINGLETON_INL
