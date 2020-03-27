class DeltaTimeNode
{
	constructor(){}
	function RunOut()
	{
		local deltaTime = GetDeltaTime();
		CreateOutput("aOutDeltaTime", deltaTime, "float");
	}
}