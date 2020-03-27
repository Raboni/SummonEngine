#pragma once

#ifdef DLL_EXPORTS
#define DLL_API
#else
#define DLL_API
#endif

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define SAFE_DELETE(aPointer)						\
	delete aPointer;								\
	aPointer = nullptr;
#define CYCLIC_ERASE(aList, aIndex)					\
	assert(aIndex >= 0 && aIndex < aList.size());	\
	if(aIndex >= 0 && aIndex < aList.size())		\
	{												\
		aList[aIndex] = aList[aList.size() - 1];	\
		aList.pop_back();							\
	}