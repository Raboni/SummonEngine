#pragma once
#include <string>

namespace CU
{
	class StateStack;
	class State;

	class StateStackProxy
	{
	public:
		StateStackProxy(StateStack* aStack);

		void SaveState(State* aState, const std::string& aName);
		void PushState(State* aState);
		void PushState(const std::string& aSavedState);

	private:
		StateStack* myStack;
	};
}