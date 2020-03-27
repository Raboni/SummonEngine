class CheckpointNode
{
	constructor(){}
	//define argTypes int
	function Run(aInt)
	{
		RegisterNodeEventListener(aInt, "OnEnter");
		return "Out";
	}
	
	function OnEnter(aInt)
	{
		UnregisterNodeEventListener(aInt, "OnEnter");
		Checkpoint(aInt);
		return "OnEnter";
	}
}