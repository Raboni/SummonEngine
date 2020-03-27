#include "stdafx.h"
#include "Editor.h"
#include "LevelEditorState.h"
#include <DebugTools.h>

Editor::Editor(CU::StateStackProxy aStateStack) : myStateStack(aStateStack)
{
}

void Editor::Init(ComponentManager* aComponentManager)
{
	if (aComponentManager == nullptr)
	{
		CU::Log("No component manager for editor found, cannot launch editor", CU::LogType::Error);
		return;
	}

	myStateStack.SaveState(new LevelEditorState(*aComponentManager), "LevelEditor");
}

void Editor::Start()
{
	myStateStack.PushState("LevelEditor");
}