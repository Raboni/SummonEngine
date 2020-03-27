#pragma once
#include "Line.hpp"
#include "GrowingArray.hpp"
#include <vector>

namespace CU
{
	template<typename T>
	class LineVolume
	{
	public:
		LineVolume();
		~LineVolume() = default;
		LineVolume(const std::vector<Line<T>>& aLineList);
		void AddLine(const Line<T>& aLine);
		bool Inside(const Vector2<T>& aPosition);

	private:
		GrowingArray<Line<T>> myLines;
	};

	template<typename T>
	inline LineVolume<T>::LineVolume()
	{
		myLines.Init(8);
	}

	template<typename T>
	inline LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myLines.Init(static_cast<unsigned short>(aLineList.size()));
		for (unsigned short index = 0; index < myLines.Capacity(); index++)
		{
			myLines.Add(aLineList[index]);
		}
	}

	template<typename T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myLines.Add(aLine);
	}

	template<typename T>
	inline bool LineVolume<T>::Inside(const Vector2<T>& aPosition)
	{
		for (unsigned short index = 0; index < myLines.Size(); index++)
		{
			if (!myLines[index].Inside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}