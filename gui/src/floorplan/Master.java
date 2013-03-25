package floorplan;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * A floorplan master is a floorplan with a known layout, but hasn't been
 * instantiated yet. In other words, it is a description of a actual floorplan
 * instance. It can consist of many sub floorplan masters. When editing a
 * floorplan, you are actually editing the floorplan master. 
 */
public class Master 
{	
	private String name;
	private double height;
	private double width;
	private boolean leaf;
	
	// List of sub floorplans
	private Vector<MasterInst> master_insts;
	// Mapping of floorplans by instance name
	private HashMap<String, MasterInst> master_map;
	
	// Constructor for a non-leaf master
	public Master(String name)
	{
		leaf = false;
		this.name = name;
		this.master_insts = new Vector<MasterInst>();
		this.master_map = new HashMap<String, MasterInst>();
	}
	
	// Constructor for a leaf master
	public Master(double width, double height)
	{
		this("BlackBox");
		leaf = true;
		this.width = width;
		this.height = height;
	}
	
	public void addMasterInst(Master sub_master, String inst_name, double x, double y)
	{
		MasterInst inst = new MasterInst(sub_master, inst_name, x, y);
		master_insts.add(inst);
		master_map.put(sub_master.getName(), inst);
	}
	
	public boolean isLeaf()
	{
		return leaf;
	}

	public double getHeight()
	{
		return height;		
	}
	
	public double getWidth()
	{
		return width;
	}
	
	public String getName()
	{
		return name;
	}
	
	public Vector<MasterInst> getFloorplanInsts()
	{
		return master_insts;
	}
	
	public HashMap<String, MasterInst> getFloorplanMap()
	{
		return master_map;
	}
	
	/**
	 * Parses masters from a file
	 * @param file
	 * @return Master
	 * @throws IOException
	 */
	public static MasterMap parseMasters(File file) throws IOException
	{
		// Create HashMap with a mapping to all known masters
		MasterMap masters = new MasterMap();
		// Populate the masters map
		parseMasters(file, masters);
		return masters;
	}
	
	private static void parseMasters(File file, MasterMap masters) throws IOException
	{
		// Until the subckt name is defined, make the master have a default name
		Master m = new Master("Default");
		MasterInst top = new MasterInst(m, "Top", 0.0, 0.0);
		// Set this top be the top master
		masters.put("Default", m);
		masters.setTop(top);
		
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
				// Right now don't do the check to see if the master already exists
				// Right now all sub masters are leafs
				// Create sub floorplan
				Master sub_master = new Master(
						Double.parseDouble(fplan_pattern_matcher.group(2)),
						Double.parseDouble(fplan_pattern_matcher.group(3)));
				
				// Add the master inst
				m.addMasterInst(sub_master, fplan_pattern_matcher.group(1), 
						Double.parseDouble(fplan_pattern_matcher.group(4)), 
						Double.parseDouble(fplan_pattern_matcher.group(5)));
			}
			else
			{
				s.close();
				throw new Error("Invalid floorplan file syntax on line " + line_num + ": '" + line + "'");
			}
		}
		
		s.close();
	}
	
}
