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
		this("Atomic");
		leaf = true;
		this.width = width;
		this.height = height;
	}
	
	public void addMasterInst(Master sub_master, String inst_name, double x, double y)
	{
		MasterInst inst = new MasterInst(sub_master, inst_name, x, y);
		master_insts.add(inst);
		master_map.put(inst_name, inst);
	}
	
	public String toString()
	{
		return name;
	}
	
	public boolean isLeaf() { return leaf; }
	public double getHeight() { return height;	}	
	public double getWidth() { return width; }	
	public String getName() { return name; }	
	public Vector<MasterInst> getFloorplanInsts() { return master_insts; }	
	public HashMap<String, MasterInst> getFloorplanMap() { return master_map; }
	
	/**
	 * Parses masters from a file
	 * @param file
	 * @return Master
	 * @throws IOException
	 */
}
