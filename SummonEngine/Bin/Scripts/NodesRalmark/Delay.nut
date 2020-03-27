dofile("scriptnodes/basenoderunner.nut");

class Delay extends BaseNodeRunner
{
	function Run(aTime)
	{
		StartDelay(aTime);
		return "Out";
	}
	
	function TimerReachedZero()
	{
		return "TimerReachedZero";
	}
	
}