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
	public static final long GRID_SCALE = 1000000000L;
	// Grid point 0
	public static final GridPoint ZERO = new GridPoint(0);
	
	// The value of the FixedPoint number, in number of GRID_STEP
	private long val;
	
	public GridPoint(double a)
	{
		this.val = (long) Math.round(a * GRID_SCALE);
	}
	
	public GridPoint(int a)
	{
		this.val = (long) Math.round(a * GRID_SCALE);
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
	
	// Overflows too often, recommend to do these kinds of multiplications by converting to double first
//	public static GridPoint mult(GridPoint a, GridPoint b)
//	{
//		return new GridPoint((long) (a.val * b.val) / GRID_SCALE);
//	}
//	
// Overflows too often, recommend to do these kinds of divisions by converting to double first
//	public static GridPoint div(GridPoint a, GridPoint b) 
//	{
//		long mult = a.val * GRID_SCALE;
//		return new GridPoint((long) (mult / b.val));
//	}

	// Special div2 function that tries to minimize overflows
	// that occur frequently with the regular divide
	public static GridPoint div2(GridPoint a) 
	{
		return new GridPoint((long) (a.val / 2L));
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
