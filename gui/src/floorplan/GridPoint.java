package floorplan;

/**
 * Grid point used for all floorplan coordinates
 * @author Drunk
 *
 */
public class GridPoint 
{
	// The smallest grid step size used in the floorplan, 1nm by default
	//public static final double GRID_STEP = 1e-9;
	// The scale of the grid
	public static final long GRID_SCALE = 1000000000;
	// Grid point 0
	public static final GridPoint ZERO = new GridPoint(0);
	
	// The value of the FixedPoint number, in number of GRID_STEP
	private long val;
	
	public GridPoint(double a)
	{
		this.val = (long) (a * GRID_SCALE);
	}
	
	public GridPoint(int a)
	{
		this.val = (long) (a * GRID_SCALE);
	}
	
	private GridPoint(long num)
	{
		this.val = num;
	}
	
	public double toDouble()
	{
		return (double) val / (double) GRID_SCALE; 
	}
	
	public String toString()
	{
		return String.valueOf(toDouble());
	}
	
	public static GridPoint parseGridPoint(String s)
	{
		return new GridPoint(Double.parseDouble(s));
	}
	
	public static GridPoint add(GridPoint a, GridPoint b)
	{
		return new GridPoint((long) (a.val + b.val));
	}
	
	public static GridPoint sub(GridPoint a, GridPoint b)
	{
		return new GridPoint((long) (a.val - b.val));
	}
	
	public static GridPoint mult(GridPoint a, GridPoint b)
	{
		return new GridPoint((long) (a.val * b.val) / GRID_SCALE);
	}
	
	public static GridPoint div(GridPoint a, GridPoint b) 
	{
		return new GridPoint((long) ((a.val * GRID_SCALE) / b.val));
	}
	
	public static GridPoint max(GridPoint a, GridPoint b)
	{
		if (a.val > b.val) return a;
		return b;
	}

	public static GridPoint min(GridPoint a, GridPoint b)
	{
		if (a.val < b.val) return a;
		return b;
	}
	
	public static boolean equals(GridPoint a, GridPoint b)
	{
		return (a.val == b.val);
	}
	
	public static boolean greaterThanOrEqualsTo(GridPoint a, GridPoint b)
	{
		return a.val >= b.val;
	}
	
	public static boolean greaterThan(GridPoint a, GridPoint b)
	{
		return a.val > b.val;
	}
	
	public static boolean lessThanOrEqualsTo(GridPoint a, GridPoint b)
	{
		return a.val <= b.val;
	}

	public static boolean lessThan(GridPoint a, GridPoint b)
	{
		return a.val < b.val;
	}
}
