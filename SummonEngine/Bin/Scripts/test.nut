enum Event
{
	Space = 0
}

function DoStuffInScript()
{
	DoStuffInCPP();
	ToggleLights();
	UnregisterEventListener(Event.Space, "DoStuffInScript", "Scripts\\test.nut");
	RegisterEventListener(Event.Space, "DoStuffInOtherScript", "Scripts\\script.nut");
}