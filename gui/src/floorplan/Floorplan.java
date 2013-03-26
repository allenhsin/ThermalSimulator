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
public class Floorplan
{
	// Floorplan master
	private Master master;
	
	// Parent floorplan instance
	private Floorplan parent;
	
	// Instance parameters
	private String name;
	private double x;	// X is in absolute space, not relative to parent
	private double y;	// Y is in absolute space, not relative to parent
	private double height;
	private double width;
	private boolean leaf;
	
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
	
	public Floorplan(Master m)
	{
		this(new MasterInst(m, "Top", 0.0, 0.0), null);
	}
	
	private void initialize(MasterInst m)
	{
		height = 0.0;
		width = 0.0;
		
		master = m.m;
		name = m.n;
		x = m.x;
		y = m.y;
		if (parent != null)
		{
			x += parent.getX();
		 	y += parent.getY();
		}
		leaf = m.m.isLeaf();

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
				if (f.getWidth() + f.getX() > (x + width))
					width = f.getWidth() + f.getX() - x;
				if (f.getHeight() + f.getY() > (y + height))
					height = f.getHeight() + f.getY() - y;
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
	public boolean isLeaf() { return leaf; }
	
	public Vector<Floorplan> getChildren() { return children; }	
	public HashMap<String, Floorplan> getChildrenMap() { return children_map; }
	
	public void addChild(Floorplan sub_floorplan)
	{
		children.add(sub_floorplan);
		children_map.put(sub_floorplan.getName(), sub_floorplan);
	}
	
}
