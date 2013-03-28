package floorplan;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.table.AbstractTableModel;
import javax.swing.tree.TreeNode;

/**
 * A floorplan master is a floorplan with a known layout, but hasn't been
 * instantiated yet. In other words, it is a description of a actual floorplan
 * instance. It can consist of many sub floorplan masters. When editing a
 * floorplan, you are actually editing the floorplan master. 
 */
public class Master extends AbstractTableModel implements TreeNode
{	
	private String name;
	private double height;
	private double width;
	private boolean atomic;
	
	// List of sub floorplans
	private Vector<MasterInst> master_insts;
	// Mapping of floorplans by instance name
	private Hashtable<String, MasterInst> master_map;
	// Instance of master in the library
	private MasterInst lib_instance;
	
	// Constructor for a non-leaf master
	public Master(String name)
	{
		atomic = false;
		this.name = name;
		this.master_insts = new Vector<MasterInst>();
		this.master_map = new Hashtable<String, MasterInst>();
	}
	
	// Constructor for a leaf master
	public Master(double width, double height)
	{
		this("Atomic");
		atomic = true;
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
	
	public boolean isAtomic() { return atomic; }
	public double getHeight() { return height;	}	
	public double getWidth() { return width; }	
	public void setName(String name) { this.name = name; }
	public String getName() { return name; }
	public MasterInst getLibInstance() { return lib_instance; }
	public void setLibInstance(MasterInst lib_instance) { this.lib_instance = lib_instance; }
	
	public Vector<MasterInst> getFloorplanInsts() { return master_insts; }	
	public Hashtable<String, MasterInst> getFloorplanMap() { return master_map; }

	public boolean isLeaf() { return atomic; }
	/**
	 * There are only 4 columns
	 */
	public int getColumnCount() { return 4; }

	/**
	 * Each sub-master instance is a row
	 */
	public int getRowCount() 
	{
		return master_insts.size();
	}

	/**
	 * Get the value of a cell in the table
	 */
	public Object getValueAt(int row, int col)
	{
		switch(col)
		{
			case 0: return master_insts.get(row).n;
			case 1: return master_insts.get(row).m.getName();
			case 2: return master_insts.get(row).x;
			case 3: return master_insts.get(row).y;
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
		}
		return null;		
	}
		
	/** 
	 * Methods that implement TreeNode functionality 
	 * Note that any elements/enumerators we return class are of type MasterInst!
	 */
	public Enumeration<MasterInst> children()
	{
		return master_insts.elements();
	}
	
	public boolean getAllowsChildren()
	{ 
		return true; 
	}
	
	public TreeNode getChildAt(int idx)
	{ 
		return master_insts.get(idx).m;
	}
	
	public int getChildCount()
	{ 
		return master_insts.size();
	}
	
	public int getIndex(TreeNode node)
	{
		return master_insts.indexOf(node);
	}
	
	public TreeNode getParent() 
	{ 
		return null; 
	}
	
}
