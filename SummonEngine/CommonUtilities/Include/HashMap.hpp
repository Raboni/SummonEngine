#pragma once
#include "Hash.hpp"
#include "GrowingArray.hpp"
#include <assert.h>
#include <new>

namespace CU
{
	template <class Key, class Value>
	class HashMap
	{
	private:
		enum class State
		{
			Empty,
			Used,
			Removed
		};

		struct Entry
		{
			Entry()
			{
				state = State::Empty;
				hash = 0;
			}
			Entry(const State aState, const Key& aKey)
			{
				state = aState;
				key = aKey;
				hash = Hash(key);
			}
			State state;
			Key key;
			Value value;
			unsigned long long hash;
		};
	public:
		class Iterator
		{
		public:
			~Iterator();
			Iterator(Entry* aEntryPointer, Entry* aEntryEndPointer);
			Iterator(const Iterator& aIterator);
			Iterator& operator=(const Iterator& aIterator);

			bool operator==(const Iterator& aIterator);
			bool operator!=(const Iterator& aIterator);
			Iterator& operator++();
			Iterator operator++(int);

			const bool IsEnd() const;
			const Key GetKey() const;
			const Value& GetValue() const;
			Value& GetValue();

		private:
			Entry* myPointer;
			Entry* myEnd;
		};
	public:
		//Skapar en HashMap med plats för aCapacity element. Den behöver inte
		//kunna växa. Blir den full så blir den full.
		HashMap(const int aCapacity);
		HashMap& operator=(const HashMap& aHashMap);
		//Stoppar in aValue under nyckeln aKey. Om det fanns ett värde innan med
		//aKey som nyckel så skrivs det värdet över. Om det lyckas returneras
		//true. Om map:en är full så görs ingenting, och false returneras.
		bool Insert(const Key& aKey, const Value& aValue);
		bool Create(const Key& aKey);
		//Plockar bort elementet med nyckeln aKey, om det finns. Om det finns
		//returneras true. Om det inte finns görs ingenting, och false
		//returneras.
		bool Remove(const Key& aKey);
		bool Delete(const Key& aKey);
		void RemoveAll();
		void DeleteAll();
		//Ger en pekare till värdet med nyckeln aKey, om det finns. Om det inte
		//finns returneras nullptr.
		const Value* Get(const Key& aKey) const;
		//Som ovan, men returnerar en icke-const-pekare.
		Value* Get(const Key& aKey);

		Iterator Begin();
		Iterator End();

	private:
		HashMap();
		void MaxArray();

		GrowingArray<Entry, unsigned int> myArray;
	};

	template<class Key, class Value>
	inline HashMap<Key, Value>::HashMap(const int aCapacity)
	{
		assert(aCapacity >= 0 && "Capacity must be larger than zero");
		if (aCapacity >= 0)
		{
			myArray.Init(aCapacity);
			MaxArray();
		}
		else
		{
			myArray.Init(16);
			MaxArray();
		}
	}
	template<class Key, class Value>
	inline HashMap<Key, Value>& HashMap<Key, Value>::operator=(const HashMap<Key, Value>& aHashMap)
	{
		myArray = aHashMap.myArray;
		return *this;
	}
	template<class Key, class Value>
	inline HashMap<Key, Value>::HashMap()
	{
		myArray.Init(16);
		MaxArray();
	}
	template<class Key, class Value>
	inline void HashMap<Key, Value>::MaxArray()
	{
		myArray.Resize(myArray.Capacity());
	}

	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Insert(const Key& aKey, const Value& aValue)
	{
		if (myArray.Capacity() == 0)
		{
			return false;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].hash == hash)
			{
				myArray[index].state = State::Used;
				memcpy_s(&myArray[index].value, sizeof(Value), &aValue, sizeof(Value));
				return true;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		do
		{
			if (myArray[index].state == State::Empty || myArray[index].state == State::Removed)
			{
				new(&myArray[index]) Entry(State::Used, aKey);
				memcpy_s(&myArray[index].value, sizeof(Value), &aValue, sizeof(Value));
				return true;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return false;
	}
	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Create(const Key& aKey)
	{
		if (myArray.Capacity() == 0)
		{
			return false;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].hash == hash)
			{
				myArray[index].state = State::Used;
				return true;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		do
		{
			if (myArray[index].state == State::Empty || myArray[index].state == State::Removed)
			{
				new(&myArray[index]) Entry(State::Used, aKey);
				return true;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return false;
	}

	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Remove(const Key& aKey)
	{
		if (myArray.Capacity() == 0)
		{
			return false;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].state == State::Empty)
			{
				return false;
			}
			if (myArray[index].hash == hash)
			{
				if (myArray[index].state == State::Used)
				{
					myArray[index].state = State::Removed;
					return true;
				}
				else
				{
					return false;
				}
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return false;
	}
	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Delete(const Key& aKey)
	{
		if (myArray.Capacity() == 0)
		{
			return false;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].state == State::Empty)
			{
				return false;
			}
			if (myArray[index].hash == hash)
			{
				if (myArray[index].state == State::Used)
				{
					delete myArray[index].value;
					myArray[index].value = nullptr;
					myArray[index].state = State::Removed;
					return true;
				}
				else
				{
					return false;
				}
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return false;
	}

	template<class Key, class Value>
	inline void HashMap<Key, Value>::RemoveAll()
	{
		for (unsigned int index = 0; index < myArray.Size(); index++)
		{
			if (myArray[index].state == State::Used)
			{
				myArray[index].state = State::Removed;
			}
		}
	}
	template<class Key, class Value>
	inline void HashMap<Key, Value>::DeleteAll()
	{
		for (unsigned int index = 0; index < myArray.Size(); index++)
		{
			if (myArray[index].state == State::Used)
			{
				delete myArray[index].value;
				myArray[index].value = nullptr;
				myArray[index].state = State::Removed;
			}
		}
	}

	template<class Key, class Value>
	inline const Value* HashMap<Key, Value>::Get(const Key& aKey) const
	{
		if (myArray.Capacity() == 0)
		{
			return nullptr;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].state == State::Empty)
			{
				return nullptr;
			}
			if (myArray[index].hash == hash)
			{
				if (myArray[index].state == State::Removed)
				{
					return nullptr;
				}
				return &myArray[index].value;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return nullptr;
	}
	template<class Key, class Value>
	inline Value* HashMap<Key, Value>::Get(const Key& aKey)
	{
		if (myArray.Capacity() == 0)
		{
			return nullptr;
		}

		unsigned long long hash = Hash(aKey);
		unsigned int startIndex = hash % myArray.Size();
		unsigned int index = startIndex;
		do
		{
			if (myArray[index].state == State::Empty)
			{
				return nullptr;
			}
			if (myArray[index].hash == hash)
			{
				if (myArray[index].state == State::Removed)
				{
					return nullptr;
				}
				return &myArray[index].value;
			}
			index++;
			if (index == myArray.Size())
			{
				index = 0;
			}
		} while (index != startIndex);
		return nullptr;
	}

	template<class Key, class Value>
	inline typename HashMap<Key, Value>::Iterator HashMap<Key, Value>::Begin()
	{
		Entry* ptr = myArray.GetPointer();
		Entry* endPtr = myArray.GetPointer() + myArray.Size();
		while (ptr != endPtr && ptr->key.length() < 1)
		{
			ptr++;
		}
		return Iterator(ptr, endPtr);
	}
	template<class Key, class Value>
	inline typename HashMap<Key, Value>::Iterator HashMap<Key, Value>::End()
	{
		Entry* ptr = myArray.GetPointer() + myArray.Size();
		return Iterator(ptr, ptr);
	}

	template<class Key, class Value>
	inline HashMap<Key, Value>::Iterator::Iterator(HashMap<Key, Value>::Entry* aEntryPointer, HashMap<Key, Value>::Entry* aEntryEndPointer)
	{
		myPointer = aEntryPointer;
		myEnd = aEntryEndPointer;
	}
	template<class Key, class Value>
	inline HashMap<Key, Value>::Iterator::~Iterator()
	{
	}

	template<class Key, class Value>
	inline HashMap<Key, Value>::Iterator::Iterator(const Iterator& aIterator)
	{
		myPointer = aIterator.myPointer;
		myEnd = aIterator.myEnd;
	}
	template<class Key, class Value>
	inline typename HashMap<Key, Value>::Iterator& HashMap<Key, Value>::Iterator::operator=(const Iterator& aIterator)
	{
		myPointer = aIterator.myPointer;
		myEnd = aIterator.myEnd;
		return *this;
	}

	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Iterator::operator==(const HashMap<Key, Value>::Iterator& aIterator)
	{
		return myPointer == aIterator.myPointer;
	}
	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Iterator::operator!=(const HashMap<Key, Value>::Iterator& aIterator)
	{
		return myPointer != aIterator.myPointer;
	}
	template<class Key, class Value>
	inline typename HashMap<Key, Value>::Iterator& HashMap<Key, Value>::Iterator::operator++()
	{
		if (myPointer == myEnd)
		{
			return *this;
		}

		do
		{
			myPointer++;
		} while (myPointer != myEnd && myPointer->key.length() < 1);
		return *this;
	}
	template<class Key, class Value>
	inline typename HashMap<Key, Value>::Iterator HashMap<Key, Value>::Iterator::operator++(int)
	{
		if (myPointer == myEnd)
		{
			return *this;
		}

		Iterator it(*this);
		do
		{
			myPointer++;
		} while (myPointer != myEnd && myPointer->key.length() < 1);
		return it;
	}
	
	template<class Key, class Value>
	inline const bool HashMap<Key, Value>::Iterator::IsEnd() const
	{
		return myPointer == myEnd;
	}
	template<class Key, class Value>
	inline const Key HashMap<Key, Value>::Iterator::GetKey() const
	{
		return myPointer->key;
	}
	template<class Key, class Value>
	inline const Value& HashMap<Key, Value>::Iterator::GetValue() const
	{
		return myPointer->value;
	}
	template<class Key, class Value>
	inline Value& HashMap<Key, Value>::Iterator::GetValue()
	{
		return myPointer->value;
	}
}