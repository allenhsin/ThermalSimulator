package floorplan;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

// A floorplan is a box containing a list of sub-floorplans positioned at specific
// coordinates as well as its own position
public class Floorplan
{
	public Floorplan(String name, double width, double height)
	{
		this.name = name;
		this.width = width;
		this.height = height;
		this.floorplan_insts = new ArrayList<FloorplanInst>();
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
	
	public ArrayList<FloorplanInst> getFloorplanInsts()
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
	
	// Floorplan name, height, width
	private String name;
	private double height;
	private double width;
	
	// List of sub floorplans
	private ArrayList<FloorplanInst> floorplan_insts;
	// Mapping of floorplans
	private HashMap<String, FloorplanInst> floorplan_map;
	
}
