enum Event
{
	Space = 0
}

function DoStuffInOtherScript()
{
	HelloWorld();
	ToggleDeferred();
	UnregisterEventListener(Event.Space, "DoStuffInOtherScript", "Scripts\\script.nut");
	RegisterEventListener(Event.Space, "DoStuffInScript", "Scripts\\test.nut");
}