package floorplan;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

import javax.swing.table.AbstractTableModel;

// A floorplan is a box containing a list of sub-floorplans positioned at specific
// coordinates as well as its own position. A floorplan can also be viewed as a 5
// column table, having a sub-floorplan name, x-coordinate, y-coordinate, and the
// height/width of each sub-floorplan
public class Floorplan extends AbstractTableModel
{
	// Floorplan name, height, width
	private String name;
	private double height;
	private double width;
	
	// List of sub floorplans
	private Vector<FloorplanInst> floorplan_insts;
	// Mapping of floorplans
	private HashMap<String, FloorplanInst> floorplan_map;
	
	public Floorplan(String name, double width, double height)
	{
		this.name = name;
		this.width = width;
		this.height = height;
		this.floorplan_insts = new Vector<FloorplanInst>();
		this.floorplan_map = new HashMap<String, FloorplanInst>();
	}
	
	public String getName()
	{
		return name;
	}
	
	public double getHeight()
	{
		return height;		
	}
	
	public double getWidth()
	{
		return width;
	}
	
	public Vector<FloorplanInst> getFloorplanInsts()
	{
		return floorplan_insts;
	}
	
	public HashMap<String, FloorplanInst> getFloorplanMap()
	{
		return floorplan_map;
	}
	
	public void addFloorplanInst(Floorplan sub_floorplan, double x, double y)
	{
		FloorplanInst inst = new FloorplanInst(sub_floorplan, x, y);
		floorplan_insts.add(inst);
		floorplan_map.put(sub_floorplan.getName(), inst);
	}
	
	// Parses the floorplan from a file
	public static Floorplan parseFloorplan(File file) throws IOException
	{
		// Setup
		Floorplan f = new Floorplan("", 0.02, 0.02);		
		int line_num = 0;
		Scanner s;
		
		// Setup regex
		Pattern fplan_pattern = Pattern.compile ("(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)");
		Matcher fplan_pattern_matcher;
		
		s = new Scanner(file);
		
		while (s.hasNextLine())
		{			
			String line = s.nextLine();
			++line_num;
			
			fplan_pattern_matcher = fplan_pattern.matcher(line);
			
			if (fplan_pattern_matcher.matches())
			{
				// Create sub floorplan
				Floorplan sub_fplan = new Floorplan(						
						fplan_pattern_matcher.group(1),
						Double.parseDouble(fplan_pattern_matcher.group(2)),
						Double.parseDouble(fplan_pattern_matcher.group(3)));
				
				// Add the sub floorplan
				f.addFloorplanInst(sub_fplan, Double.parseDouble(fplan_pattern_matcher.group(4)), Double.parseDouble(fplan_pattern_matcher.group(5)));
			}
			else throw new Error("Invalid floorplan file syntax on line " + line_num + ": '" + line + "'");
		}
		
		s.close();

		return f;
	}
	
	/**
	 * There are only 5 columns
	 */
	public int getColumnCount() 
	{
		return 5;
	}

	/**
	 * Each sub-floorplan instance is a row
	 */
	public int getRowCount() 
	{
		return floorplan_insts.size();
	}

	/**
	 * Get the value of a cell in the table
	 */
	public Object getValueAt(int row, int col)
	{
		switch(col)
		{
			case 0: return floorplan_insts.get(row).f.getName();
			case 1: return floorplan_insts.get(row).x;
			case 2: return floorplan_insts.get(row).y;
			case 3: return floorplan_insts.get(row).f.getWidth();
			case 4: return floorplan_insts.get(row).f.getHeight();
		}
		return null;
	}
	
	public String getColumnName(int col)
	{
		switch(col)
		{
			case 0: return "Name";
			case 1: return "X";
			case 2: return "Y";
			case 3: return "Width";
			case 4: return "Height";
		}
		return null;		
	}
	
}
