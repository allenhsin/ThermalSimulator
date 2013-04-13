package floorplan;

/**
 * Simple class that represents a box
 */
public class Box
{
	public GridPoint llx, lly, urx, ury;
	
	public Box(double llx, double lly, double urx, double ury)
	{
		this.llx = new GridPoint(llx);
		this.lly = new GridPoint(lly);
		this.urx = new GridPoint(urx);
		this.ury = new GridPoint(ury);
	}
	
	public Box(GridPoint llx, GridPoint lly, GridPoint urx, GridPoint ury)
	{
		this.llx = llx;
		this.lly = lly;
		this.urx = urx;
		this.ury = ury;
	}
	
	public GridPoint getHeight()
	{
		return GridPoint.sub(ury, lly);
	}
	public GridPoint getWidth()
	{
		return GridPoint.sub(urx, llx);
	}
	
	public boolean isValidBox()
	{
		if (GridPoint.greaterThanOrEqualsTo(llx, urx))
			return false;
		if (GridPoint.greaterThanOrEqualsTo(lly, ury))
			return false;
		return true;
	}
}

