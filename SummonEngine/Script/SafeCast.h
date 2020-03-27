#pragma once
#include <any>

template<class T>
inline T SafeCast(const std::any& aInput)
{
	try
	{
		T casted = std::any_cast<T>(aInput);
		return casted;
	}
	catch (const std::bad_any_cast& /*e*/)
	{
		std::cout << "Safe cast failed, C++ type " << typeid(T).name() << " Squirrel type: " << aInput.type().name() << std::endl;
	}
	return T();
}