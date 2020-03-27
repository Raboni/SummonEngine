#pragma once
#include "StateStackProxy.hpp"

namespace CU
{
	enum class StateAction
	{
		None,
		Pop,
		PopDelete,
		DoublePop,
		DoublePopDelete,
		PreviousPop,
		PreviousPopDelete
	};

	class State
	{
	public:
		State();
		State(StateStack* aStack);
		State(const StateStackProxy& aStack);
		virtual ~State();

		bool UpdatePrevious() const;
		bool DrawPrevious() const;
		bool IsLoaded() const;

	protected:
		virtual StateAction Update(const float aDeltaTime);
		virtual void Draw();

		virtual void Load();
		virtual void Unload();

		virtual void GainFocus();
		virtual void LoseFocus();

		bool GetIsOnTop() const;

	protected:
		StateStackProxy myStack;
		bool myUpdatePrevious;
		bool myDrawPrevious;

	private:
		friend class StateStack;

		bool myLoaded;
		bool myIsOnTop;
	};
}