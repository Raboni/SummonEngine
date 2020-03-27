#pragma once
#include "PostMaster.hpp"
#include "Input.hpp"
#include "HashMap.hpp"
#include "Singleton.hpp"
#include "StaticArray.hpp"
#define MapArraySize 4

namespace CU
{
	enum class InputEventTrigger
	{
		Pressed,
		Down,
		Released,
		Default
	};

	template<typename EventTypeEnum>
	class InputMapper
	{
	public:
		static void CreateInstance();
		static InputMapper<EventTypeEnum>* GetInstance();
		static void DestroyInstance();

		void Subscribe(EventTypeEnum aEventType, Subscriber<EventTypeEnum>* aSubscriber);
		void Unsubscribe(Subscriber<EventTypeEnum>* aSubscriber);
		bool MapInput(const Input::KeyCode& aKeyCode, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger = InputEventTrigger::Default);
		bool MapInput(const Input::MouseButton& aMouseButton, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger = InputEventTrigger::Default);
		bool MapInput(const Input::XboxButton& aButton, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger = InputEventTrigger::Default);
		//void UnmapInput(const Input::KeyCode& aKeyCode, const EventTypeEnum& aEvent);
		//void UnmapInput(const Input::MouseButton& aMouseButton, const EventTypeEnum& aEvent);
		void Update();

	private:
		InputMapper();
		~InputMapper() = default;
		//void SendEvent(const InputEvent<EventTypeEnum>& aMessage);
		void SendEvent(const EventTypeEnum& aMessage);

		static InputMapper<EventTypeEnum>* ourInstance;

		GrowingArray<GrowingArray<Subscriber<EventTypeEnum>*, int>, int> mySubscribers;
		HashMap<EventTypeEnum, StaticArray<Input::KeyCode, MapArraySize>> myKeyboardMap;
		HashMap<EventTypeEnum, StaticArray<Input::MouseButton, MapArraySize>> myMouseMap;
		HashMap<EventTypeEnum, StaticArray<Input::XboxButton, MapArraySize>> myXboxMap;
		InputEventTrigger myEventTriggerArray[static_cast<int>(EventTypeEnum::Count)];
	};
	template<typename EventTypeEnum>
	InputMapper<EventTypeEnum>* InputMapper<EventTypeEnum>::ourInstance = nullptr;

	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::CreateInstance()
	{
		if (ourInstance == nullptr)
		{
			ourInstance = new InputMapper<EventTypeEnum>();
		}
	}
	template<typename EventTypeEnum>
	inline InputMapper<EventTypeEnum>* InputMapper<EventTypeEnum>::GetInstance()
	{
		return ourInstance;
	}
	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::DestroyInstance()
	{
		if (ourInstance != nullptr)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}

	template<typename EventTypeEnum>
	inline InputMapper<EventTypeEnum>::InputMapper() : myKeyboardMap(30), myMouseMap(5), myXboxMap(14)
	{
		int enumCount = static_cast<int>(EventTypeEnum::Count);
		mySubscribers.Init(enumCount);
		for (int typeIndex = 0; typeIndex < enumCount; typeIndex++)
		{
			mySubscribers.Add(GrowingArray<Subscriber<EventTypeEnum>*>(4));
		}
		for (int eventIndex = 0; eventIndex < enumCount; eventIndex++)
		{
			myEventTriggerArray[eventIndex] = InputEventTrigger::Pressed;
		}
	}

	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::Subscribe(EventTypeEnum aMessageType, Subscriber<EventTypeEnum>* aSubscriber)
	{
		int index = static_cast<int>(aMessageType);
		assert(mySubscribers.Size() > index);
		if (mySubscribers.Size() > index)
		{
			mySubscribers[index].Add(aSubscriber);
		}
	}
	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::Unsubscribe(Subscriber<EventTypeEnum>* aSubscriber)
	{
		for (int messageIndex = 0; messageIndex < mySubscribers.Size(); messageIndex++)
		{
			for (int subIndex = 0; subIndex < mySubscribers[messageIndex].Size(); subIndex++)
			{
				if (mySubscribers[messageIndex][subIndex] == aSubscriber)
				{
					mySubscribers[messageIndex].RemoveCyclicAtIndex(subIndex--);
				}
			}
		}
	}
	/*template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::SendEvent(const Message<EventTypeEnum>& aMessage)
	{
		unsigned short typeIndex = static_cast<unsigned short>(aMessage.GetType());
		assert(mySubscribers.Size() > typeIndex);
		if (mySubscribers.Size() > typeIndex)
		{
			for (unsigned short index = 0; index < mySubscribers[typeIndex].Size(); index++)
			{
				mySubscribers[typeIndex][index]->RecieveMessage(aMessage);
			}
		}
	}*/
	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::SendEvent(const EventTypeEnum& aMessage)
	{
		int typeIndex = static_cast<int>(aMessage);
		assert(mySubscribers.Size() > typeIndex);
		if (mySubscribers.Size() > typeIndex)
		{
			Message<EventTypeEnum> message(aMessage);
			for (int index = 0; index < mySubscribers[typeIndex].Size(); index++)
			{
				mySubscribers[typeIndex][index]->RecieveMessage(message);
			}
		}
	}
	template<typename EventTypeEnum>
	inline bool InputMapper<EventTypeEnum>::MapInput(const Input::KeyCode& aKeyCode, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger)
	{
		if (aTrigger != InputEventTrigger::Default)
		{
			myEventTriggerArray[static_cast<int>(aEvent)] = aTrigger;
		}

		StaticArray<Input::KeyCode, MapArraySize>* mapPointer = myKeyboardMap.Get(aEvent);
		if (mapPointer == nullptr)
		{
			myKeyboardMap.Insert(aEvent, StaticArray<Input::KeyCode, MapArraySize>());
			mapPointer = myKeyboardMap.Get(aEvent);
		}
		for (int i = 0; i < MapArraySize; i++)
		{
			if ((*mapPointer)[i] == static_cast<Input::KeyCode>(0))
			{
				(*mapPointer)[i] = aKeyCode;
				return true;
			}
		}
		return false;
	}
	template<typename EventTypeEnum>
	inline bool InputMapper<EventTypeEnum>::MapInput(const Input::MouseButton& aMouseButton, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger)
	{
		if (aTrigger != InputEventTrigger::Default)
		{
			myEventTriggerArray[static_cast<int>(aEvent)] = aTrigger;
		}

		StaticArray<Input::MouseButton, MapArraySize>* mapPointer = myMouseMap.Get(aEvent);
		if (mapPointer == nullptr)
		{
			myMouseMap.Insert(aEvent, StaticArray<Input::MouseButton, MapArraySize>());
			mapPointer = myMouseMap.Get(aEvent);
		}
		for (int i = 0; i < MapArraySize; i++)
		{
			if ((*mapPointer)[i] == static_cast<Input::MouseButton>(0))
			{
				(*mapPointer)[i] = aMouseButton;
				return true;
			}
		}
		return false;
	}
	template<typename EventTypeEnum>
	inline bool InputMapper<EventTypeEnum>::MapInput(const Input::XboxButton& aButton, const EventTypeEnum& aEvent, const InputEventTrigger& aTrigger)
	{
		if (aTrigger != InputEventTrigger::Default)
		{
			myEventTriggerArray[static_cast<int>(aEvent)] = aTrigger;
		}

		StaticArray<Input::XboxButton, MapArraySize>* mapPointer = myXboxMap.Get(aEvent);
		if (mapPointer == nullptr)
		{
			myXboxMap.Insert(aEvent, StaticArray<Input::XboxButton, MapArraySize>());
			mapPointer = myXboxMap.Get(aEvent);
		}
		for (int i = 0; i < MapArraySize; i++)
		{
			if ((*mapPointer)[i] == static_cast<Input::XboxButton>(0))
			{
				(*mapPointer)[i] = aButton;
				return true;
			}
		}
		return false;
	}
	template<typename EventTypeEnum>
	inline void InputMapper<EventTypeEnum>::Update()
	{
		for (int eventIndex = 0; eventIndex < EnumAmount; eventIndex++)
		{
			StaticArray<Input::KeyCode, MapArraySize>* keyboardMapPointer = myKeyboardMap.Get(static_cast<EventTypeEnum>(eventIndex));
			if (keyboardMapPointer != nullptr)
			{
				StaticArray<Input::KeyCode, MapArraySize> keyboardArray = *keyboardMapPointer;
				for (int keyIndex = 0; keyIndex < MapArraySize; keyIndex++)
				{
					if (keyboardArray[keyIndex] == static_cast<Input::KeyCode>(0))
					{
						break;
					}
					switch (myEventTriggerArray[eventIndex])
					{
					case InputEventTrigger::Down:
						if (Input::Get()->GetKeyDown(keyboardArray[keyIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Released:
						if (Input::Get()->GetKeyReleased(keyboardArray[keyIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Pressed:
					default:
						if (Input::Get()->GetKeyPressed(keyboardArray[keyIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					}
				}
			}
			StaticArray<Input::MouseButton, MapArraySize>* mouseMapPointer = myMouseMap.Get(static_cast<EventTypeEnum>(eventIndex));
			if (mouseMapPointer != nullptr)
			{
				StaticArray<Input::MouseButton, MapArraySize> mouseArray = *mouseMapPointer;
				for (int buttonIndex = 0; buttonIndex < MapArraySize; buttonIndex++)
				{
					if (mouseArray[buttonIndex] == static_cast<Input::MouseButton>(0))
					{
						break;
					}
					switch (myEventTriggerArray[eventIndex])
					{
					case InputEventTrigger::Down:
						if (Input::Get()->GetMouseButtonDown(mouseArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Released:
						if (Input::Get()->GetMouseButtonReleased(mouseArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Pressed:
					default:
						if (Input::Get()->GetMouseButtonPressed(mouseArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					}
				}
			}
			StaticArray<Input::XboxButton, MapArraySize>* xboxMapPointer = myXboxMap.Get(static_cast<EventTypeEnum>(eventIndex));
			if (xboxMapPointer != nullptr)
			{
				StaticArray<Input::XboxButton, MapArraySize> xboxArray = *xboxMapPointer;
				for (int buttonIndex = 0; buttonIndex < MapArraySize; buttonIndex++)
				{
					if (xboxArray[buttonIndex] == static_cast<Input::XboxButton>(0))
					{
						break;
					}
					switch (myEventTriggerArray[eventIndex])
					{
					case InputEventTrigger::Down:
						if (Input::Get()->GetXboxDown(xboxArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Released:
						if (Input::Get()->GetXboxReleased(xboxArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					case InputEventTrigger::Pressed:
					default:
						if (Input::Get()->GetXboxPressed(xboxArray[buttonIndex]))
						{
							SendEvent(static_cast<EventTypeEnum>(eventIndex));
						}
						break;
					}
				}
			}
		}
	}
}