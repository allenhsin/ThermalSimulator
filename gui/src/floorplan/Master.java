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
public class Master implements Comparable
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
	
	/**
	 * Add an instance
	 */
	public void addMasterInst(MasterInst sub_inst)
	{
		master_insts.add(sub_inst);
		master_map.put(sub_inst.n, sub_inst);
	}
	
	public void addMasterInst(Master sub_master, String inst_name, double x, double y)
	{
		addMasterInst(new MasterInst(sub_master, inst_name, x, y));
	}

	/**
	 * Delete an instance
	 */
	public void deleteMasterInst(MasterInst sub_inst)
	{
		master_insts.remove(sub_inst);
		master_map.remove(sub_inst);
	}
	
	public void deleteMasterInst(String name)
	{
		deleteMasterInst(master_map.get(name));
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
	
	public Vector<MasterInst> getInstances() { return master_insts; }	
	public Hashtable<String, MasterInst> getInstanceMap() { return master_map; }

	public int compareTo(Object other_obj)
	{
		return name.compareTo(other_obj.toString());		
	}
	
}

