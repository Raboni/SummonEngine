dofile("scriptnodes/basenodegrabber.nut");

class GetTotalTime extends BaseNodeGrabber
{
	function Run()
	{
		local totalTime = CPPGetTotalTime();
		CreateOutput("DT", totalTime);
	}
	
}