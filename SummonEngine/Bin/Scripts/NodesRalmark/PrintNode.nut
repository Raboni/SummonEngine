dofile("scriptnodes/basenoderunner.nut");

class Print extends BaseNodeRunner
{
	constructor(){}
	function Run(aString)
	{
		
		print(aString);
		return "Out";
	}
	
}