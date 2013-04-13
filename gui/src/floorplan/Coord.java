package floorplan;

/**
 * Simple class with X, Y fixed point coordinates
 */
public class Coord
{
	public GridPoint x, y;
	
	public Coord(GridPoint x, GridPoint y)
	{
		this.x = x; 
		this.y = y; 
	}
	
	public Coord(double x, double y)
	{ 
		this.x = new GridPoint(x); 
		this.y = new GridPoint(y); 
	}
}
