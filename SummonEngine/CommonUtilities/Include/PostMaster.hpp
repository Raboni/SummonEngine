#pragma once
#include "Message.hpp"
#include "Subscriber.hpp"
#include "GrowingArray.hpp"

namespace CU
{
	template<typename MessageTypeEnum>
	class PostMaster
	{
	public:
		static void CreateInstance();
		static PostMaster<MessageTypeEnum>* GetInstance();
		static void DeleteInstance();

		void Subscribe(MessageTypeEnum aMessageType, Subscriber<MessageTypeEnum>* aSubscriber);
		void Unsubscribe(Subscriber<MessageTypeEnum>* aSubscriber);
		void Unsubscribe(Subscriber<MessageTypeEnum>* aSubscriber, const MessageTypeEnum& aMessageType);
		void SendInstantMessage(const Message<MessageTypeEnum>& aMessage);
		void SendInstantMessage(const MessageTypeEnum& aMessage);

		void SendDelayedMessage(const Message<MessageTypeEnum>& aMessage);
		void Dispatch();

	private:
		PostMaster();
		~PostMaster() = default;

	private:
		static PostMaster<MessageTypeEnum>* ourInstance;

		GrowingArray<GrowingArray<Subscriber<MessageTypeEnum>*>, unsigned short> mySubscribers;
		GrowingArray<Message<MessageTypeEnum>, unsigned short> myDelayedMessages;
	};
	template<typename MessageTypeEnum>
	PostMaster<MessageTypeEnum>* PostMaster<MessageTypeEnum>::ourInstance = nullptr;

	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::CreateInstance()
	{
		if (ourInstance == nullptr)
		{
			ourInstance = new PostMaster<MessageTypeEnum>();
		}
	}
	template<typename MessageTypeEnum>
	inline PostMaster<MessageTypeEnum>* PostMaster<MessageTypeEnum>::GetInstance()
	{
		return ourInstance;
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::DeleteInstance()
	{
		if (ourInstance != nullptr)
		{
			delete ourInstance;
			ourInstance = nullptr;
		}
	}

	template<typename MessageTypeEnum>
	inline PostMaster<MessageTypeEnum>::PostMaster()
	{
		mySubscribers.Init(static_cast<int>(MessageTypeEnum::Count));
		for (unsigned int typeIndex = 0; typeIndex < static_cast<int>(MessageTypeEnum::Count); typeIndex++)
		{
			mySubscribers.Add(GrowingArray<Subscriber<MessageTypeEnum>*>(4));
		}
		myDelayedMessages.Init(4);
	}

	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::Subscribe(MessageTypeEnum aMessageType, Subscriber<MessageTypeEnum>* aSubscriber)
	{
		unsigned short index = static_cast<unsigned short>(aMessageType);
		assert(mySubscribers.Size() > index);
		if (mySubscribers.Size() > index)
		{
			mySubscribers[index].Add(aSubscriber);
		}
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::Unsubscribe(Subscriber<MessageTypeEnum>* aSubscriber)
	{
		for (unsigned short messageIndex = 0; messageIndex < mySubscribers.Size(); messageIndex++)
		{
			for (unsigned short subIndex = 0; subIndex < mySubscribers[messageIndex].Size(); subIndex++)
			{
				if (mySubscribers[messageIndex][subIndex] == aSubscriber)
				{
					mySubscribers[messageIndex].RemoveCyclicAtIndex(subIndex--);
				}
			}
		}
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::Unsubscribe(Subscriber<MessageTypeEnum>* aSubscriber, const MessageTypeEnum& aMessageType)
	{
		unsigned short messageIndex = static_cast<unsigned short>(aMessageType);
		for (unsigned short subIndex = 0; subIndex < mySubscribers[messageIndex].Size(); subIndex++)
		{
			if (mySubscribers[messageIndex][subIndex] == aSubscriber)
			{
				mySubscribers[messageIndex].RemoveCyclicAtIndex(subIndex--);
			}
		}
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::SendInstantMessage(const Message<MessageTypeEnum>& aMessage)
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
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::SendInstantMessage(const MessageTypeEnum& aMessage)
	{
		unsigned short typeIndex = static_cast<unsigned short>(aMessage);
		assert(mySubscribers.Size() > typeIndex);
		if (mySubscribers.Size() > typeIndex)
		{
			Message<MessageTypeEnum> message(aMessage);
			for (unsigned short index = 0; index < mySubscribers[typeIndex].Size(); index++)
			{
				mySubscribers[typeIndex][index]->RecieveMessage(message);
			}
		}
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::SendDelayedMessage(const Message<MessageTypeEnum>& aMessage)
	{
		myDelayedMessages.Add(aMessage);
	}
	template<typename MessageTypeEnum>
	inline void PostMaster<MessageTypeEnum>::Dispatch()
	{
		for (unsigned short messageIndex = 0; messageIndex < myDelayedMessages.Size(); messageIndex++)
		{
			Message<MessageTypeEnum>& message = myDelayedMessages[messageIndex];
			unsigned short messageTypeIndex = static_cast<unsigned short>(message.GetType());
			for (unsigned short subscriberIndex = 0; subscriberIndex < mySubscribers[messageTypeIndex].Size(); subscriberIndex++)
			{
				mySubscribers[messageTypeIndex][subscriberIndex]->RecieveMessage(message);
			}

		}
		myDelayedMessages.RemoveAll();
	}
}