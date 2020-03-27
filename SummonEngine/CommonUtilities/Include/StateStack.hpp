#pragma once
#include "State.hpp"
#include "GrowingArray.hpp"
#include "HashMap.hpp"
#include <string>

namespace CU
{
	class StateStack
	{
	public:
		StateStack();
		~StateStack();

		bool Update(const float aDeltaTime);
		void Draw();

		void SaveState(State* aState, const std::string& aName);
		void PushState(State* aState);
		void PushState(const std::string& aSavedState);
		void PopState(const unsigned short aStateIndex, const bool aDelete);
		void PopState(const bool aDelete);
		void Clear(const bool aDelete);
		void ClearSavedStates(const bool aDelete);

	protected:
		HashMap<std::string, State*> myStateMap;
		GrowingArray<State*, unsigned short> myStack;
		GrowingArray<StateAction, unsigned short> myActionStack;
	};
}