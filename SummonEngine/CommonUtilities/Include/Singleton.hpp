#pragma once

namespace CU
{
	template<class T>
	class Singleton
	{
	public:
		static void CreateInstance();
		static T* GetInstance();
		static void DeleteInstance();

	private:
		Singleton() = default;
		~Singleton() = default;

		static T* myInstance;
	};
	template<class T>
	T* Singleton<T>::myInstance = nullptr;

	template<class T>
	inline void Singleton<T>::CreateInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
		myInstance = new T();
	}
	template<class T>
	inline T* Singleton<T>::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new T();
		}
		return myInstance;
	}
	template<class T>
	inline void Singleton<T>::DeleteInstance()
	{
		delete myInstance;
		myInstance = nullptr;
	}
}