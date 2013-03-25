package floorplan;

/**
 * Simple class 
 *
 */
public class MasterInst
{
	// Instance of the master
	public Master m;
	// Instance name
	public String n;
	// Instance location (x, y)
	public double x, y;
	
	public MasterInst(Master m, String n, double x, double y)
	{
		this.m = m;
		this.n = n;
		this.x = x;
		this.y = y;		
	}
	
}
