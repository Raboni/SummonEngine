#pragma once
#include "GrowingArray.hpp"
#include <new>

constexpr int MessageArraySize = 128;

namespace CU
{
	struct DataRange
	{
		DataRange()
		{
			Index = -1;
			Size = -1;
		};
		DataRange(int aIndex, int aSize)
		{
			Index = aIndex;
			Size = aSize;
		};

		int Index;
		int Size;
	};

	template<typename MessageTypeEnum>
	class Message
	{
	public:
		Message();
		Message(const MessageTypeEnum& aType);
		template<typename DataType>
		Message(const MessageTypeEnum& aType, const DataType& aData);
		~Message();

		const MessageTypeEnum& GetType() const;
		template<typename DataType>
		DataType* GetData();
		template<typename DataType>
		const DataType* GetData() const;
		void* GetData();
		const void* GetData() const;

	private:
		MessageTypeEnum myType;
		int myDataRangeIndex;
		char* myData;
		static GrowingArray<DataRange, int> ourDataRangeList;
		static GrowingArray<int, int> ourDataIndexOpenList;
		static char* ourData;
		static int ourLastDataIndex;
	};
	template<typename MessageTypeEnum>
	GrowingArray<DataRange, int> Message<MessageTypeEnum>::ourDataRangeList(MessageArraySize / 4);
	template<typename MessageTypeEnum>
	GrowingArray<int, int> Message<MessageTypeEnum>::ourDataIndexOpenList(MessageArraySize / 8);
	template<typename MessageTypeEnum>
	char* Message<MessageTypeEnum>::ourData = new char[MessageArraySize];
	template<typename MessageTypeEnum>
	int Message<MessageTypeEnum>::ourLastDataIndex = -1;

	template<typename MessageTypeEnum>
	inline Message<MessageTypeEnum>::Message()
	{
		myDataRangeIndex = -1;
		myData = nullptr;
	}
	template<typename MessageTypeEnum>
	inline Message<MessageTypeEnum>::Message(const MessageTypeEnum& aType)
	{
		myType = aType;
		myDataRangeIndex = -1;
		myData = nullptr;
	}
	template<typename MessageTypeEnum>
	inline Message<MessageTypeEnum>::~Message()
	{
		if (myDataRangeIndex != -1)
		{
			ourDataIndexOpenList.Add(myDataRangeIndex);
		}
		/*if (myData != nullptr)
		{
			delete[] myData;
			myData = nullptr;
		}*/
	}
	template<typename MessageTypeEnum>
	template<typename DataType>
	inline Message<MessageTypeEnum>::Message(const MessageTypeEnum& aType, const DataType& aData)
	{
		myType = aType;
		//myData = new char[sizeof(aData)];
		//new(myData) DataType(aData);

		assert(ourDataIndexOpenList.Size() > 0 || ourDataRangeList.Size() == 0 && "message array full");
		if (ourDataIndexOpenList.Size() > 0)
		{
			//find lowest range
			int lowestOpenListIndex = -1;
			int size = sizeof(aData);
			for (int i = 0; i < ourDataIndexOpenList.Size(); i++)//may optimize with reverse heap
			{
				if ((lowestOpenListIndex == -1 || ourDataRangeList[ourDataIndexOpenList[i]].Index < ourDataRangeList[ourDataIndexOpenList[lowestOpenListIndex]].Index)
					&& ourDataRangeList[ourDataIndexOpenList[i]].Size >= size)
				{
					lowestOpenListIndex = i;
				}
			}
			assert(lowestOpenListIndex != -1 && "no open index large enough");
			if (lowestOpenListIndex == -1)
			{
				return;
			}
			DataRange lowestDataRange = ourDataRangeList[ourDataIndexOpenList[lowestOpenListIndex]];
			assert(lowestDataRange.Index >= 0 && lowestDataRange.Index + lowestDataRange.Size <= MessageArraySize && "open index out of scope");
			assert(lowestDataRange.Size >= size && "open range too small");
			if (lowestDataRange.Index >= 0 && lowestDataRange.Index < MessageArraySize)
			{
				myDataRangeIndex = ourDataIndexOpenList[lowestOpenListIndex];
				ourDataIndexOpenList.RemoveCyclicAtIndex(lowestOpenListIndex);
				//add new range if it isn't out of scope
				if (lowestDataRange.Index == ourLastDataIndex)
				{
					ourLastDataIndex = lowestDataRange.Index + size;
					assert(MessageArraySize - ourLastDataIndex >= 0 && "Message data array full");
					if (MessageArraySize - ourLastDataIndex > 0)
					{
						ourDataIndexOpenList.Add(ourDataRangeList.Size());
						ourDataRangeList.Add(DataRange(ourLastDataIndex, MessageArraySize - ourLastDataIndex));
					}
				}
				else
				{
					//int betweenSize = ourDataRangeList[ourDataIndexOpenList[lowestOpenListIndex] + 1].Index - (myDataRangeIndex + size);
					int betweenSize = lowestDataRange.Size - size;
					if (betweenSize > 0)
					{
						ourDataIndexOpenList.Add(ourDataRangeList.Size());
						ourDataRangeList.Add(DataRange(lowestDataRange.Index + size, betweenSize));
					}
				}
				myData = &ourData[lowestDataRange.Index];
				new(myData) DataType(aData);
			}
		}
		else if (ourDataRangeList.Size() == 0)
		{
			//first message
			int size = sizeof(aData);
			ourDataRangeList.Add(DataRange(0, size));
			ourDataRangeList.Add(DataRange(size, MessageArraySize - size));
			ourDataIndexOpenList.Add(1);
			ourLastDataIndex = size;
			myDataRangeIndex = 0;
			myData = ourData;
			new(myData) DataType(aData);
		}
	}
	template<typename MessageTypeEnum>
	inline const MessageTypeEnum& Message<MessageTypeEnum>::GetType() const
	{
		return myType;
	}
	template<typename MessageTypeEnum>
	template<typename DataType>
	inline DataType* Message<MessageTypeEnum>::GetData()
	{
		return reinterpret_cast<DataType*>(myData);
	}
	template<typename MessageTypeEnum>
	template<typename DataType>
	inline const DataType* Message<MessageTypeEnum>::GetData() const
	{
		return reinterpret_cast<DataType*>(myData);
	}
	template<typename MessageTypeEnum>
	inline void* Message<MessageTypeEnum>::GetData()
	{
		return myData;
	}
	template<typename MessageTypeEnum>
	inline const void* Message<MessageTypeEnum>::GetData() const
	{
		return myData;
	}
}