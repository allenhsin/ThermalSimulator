package floorplan;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

import javax.swing.table.AbstractTableModel;
import javax.swing.tree.TreeNode;

// A floorplan is a box containing a list of sub-floorplans positioned at specific
// coordinates as well as its own position. A floorplan can also be viewed as a 5
// column table, having a sub-floorplan name, x-coordinate, y-coordinate, and the
// height/width of each sub-floorplan
public class Floorplan extends AbstractTableModel
{
	// Floorplan master
	private Master master;
	
	// Parent floorplan instance
	private Floorplan parent;
	
	// Instance parameters
	private String name;
	private double x;
	private double y;
	private double height;
	private double width;
	
	// List of child floorplans
	private Vector<Floorplan> children;
	// Mapping of child floorplans
	private HashMap<String, Floorplan> children_map;
	
	public Floorplan(MasterInst m, Floorplan parent)
	{
		this.parent = parent;
		this.children = new Vector<Floorplan>();
		this.children_map = new HashMap<String, Floorplan>();
		initialize(m);
	}
	
	private void initialize(MasterInst m)
	{
		height = 0.0;
		width = 0.0;
		
		master = m.m;
		name = m.n;
		x = m.x;
		y = m.y;
		
		// If the master is a leaf, then just set height and width and be done
		if (m.m.isLeaf())
		{
			height = m.m.getHeight();
			width = m.m.getWidth();
		}
		else
		{
			// Otherwise create floorplans from the master
			Iterator<MasterInst> it = m.m.getFloorplanInsts().iterator();
			while (it.hasNext())
			{
				MasterInst master_inst = it.next();
				Floorplan f = new Floorplan(master_inst, this);
				addChild(f);
				
				// Update height/width
				if (f.getWidth() + f.getX() > width)
					width = f.getWidth() + f.getX();
				if (f.getHeight() + f.getY() > height)
					height = f.getHeight() + f.getY();
			}
		}
	}
	
	public String toString()
	{
		return name + " : " + master.getName(); 
	}
	
	public Master getMaster() { return master; }
	public String getName() { return name; }	
	public double getX() { return x; }
	public double getY() { return y; }
	public double getWidth() { return width; }
	public double getHeight() { return height; }
	
	public Vector<Floorplan> getChildren() { return children; }	
	public HashMap<String, Floorplan> getChildrenMap() { return children_map; }
	
	public void addChild(Floorplan sub_floorplan)
	{
		children.add(sub_floorplan);
		children_map.put(sub_floorplan.getName(), sub_floorplan);
	}
	
	/**
	 * There are only 6 columns
	 */
	public int getColumnCount() 
	{
		return 6;
	}

	/**
	 * Each sub-floorplan instance is a row
	 */
	public int getRowCount() 
	{
		return children.size();
	}

	/**
	 * Get the value of a cell in the table
	 */
	public Object getValueAt(int row, int col)
	{
		switch(col)
		{
			case 0: return children.get(row).name;
			case 1: return children.get(row).getMaster().getName();
			case 2: return children.get(row).x;
			case 3: return children.get(row).y;
			case 4: return children.get(row).width;
			case 5: return children.get(row).height;
		}
		return null;
	}
	
	public String getColumnName(int col)
	{
		switch(col)
		{
			case 0: return "Instance";
			case 1: return "Master";
			case 2: return "X";
			case 3: return "Y";
			case 4: return "Width";
			case 5: return "Height";
		}
		return null;		
	}
	
}
