#pragma once
#include "Message.hpp"

namespace CU
{
	template<typename MessageTypeEnum>
	class Subscriber
	{
	public:
		Subscriber() = default;
		virtual ~Subscriber() = default;

		virtual void RecieveMessage(const Message<MessageTypeEnum>& aMessage) = 0;

	private:

	};
}