package floorplan;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

/**
 * A floorplan master is a floorplan with a known layout, but hasn't been
 * instantiated yet. In other words, it is a description of a actual floorplan
 * instance. It can consist of many sub floorplan masters. When editing a
 * floorplan, you are actually editing the floorplan master. 
 */
public class Master implements Comparable<Master>
{	
	// Hierarchy separator
	public static final String HIER_SEPARATOR = ".";
	
	private String name;
	private GridPoint height;
	private GridPoint width;
	private boolean atomic;
	private boolean filler;
	
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
		this.height = GridPoint.ZERO;
		this.width = GridPoint.ZERO;
	}
	
	// Constructor for an atomic master
	public Master(GridPoint width, GridPoint height, boolean filler)
	{
		atomic = true;
		this.filler = filler;
		if (filler) name = "Filler";
		else name = "Atomic";		
		this.master_insts = new Vector<MasterInst>();
		this.master_map = new Hashtable<String, MasterInst>();
		this.width = width;
		this.height = height;
	}
	
	/**
	 * Add an instance, returns whether it was successful
	 */
	public boolean addMasterInst(MasterInst sub_inst)
	{
		// Unsuccessful if key already exists
		if (master_map.containsKey(sub_inst.n))
			return false;
		// Otherwise, add the instance
		sub_inst.parent = this;
		master_insts.add(sub_inst);
		master_map.put(sub_inst.n, sub_inst);
		return true;
	}
	
	public boolean addMasterInst(Master sub_master, String inst_name, GridPoint x, GridPoint y)
	{
		return addMasterInst(new MasterInst(this, sub_master, inst_name, x, y));
	}

	/**
	 * Delete an instance, returns whether it was successful
	 */
	public boolean deleteMasterInst(MasterInst sub_inst)
	{
		// Unsuccessful if key does not exist
		if (!master_map.containsKey(sub_inst.n))
			return false;
		// Otherwise, delete the instance
		master_insts.remove(sub_inst);
		master_map.remove(sub_inst.n);
		return true;
	}
	
	public boolean deleteMasterInst(String name)
	{
		return deleteMasterInst(master_map.get(name));
	}
	
	/**
	 * Renames a master instance
	 */
	public boolean renameMasterInst(String old_name, String new_name)
	{
		// Unsuccessful if old name does not exist, or new name exists
		if (!master_map.containsKey(old_name) || master_map.containsKey(new_name))
			return false;
		
		MasterInst inst = master_map.get(old_name);
		inst.n = new_name;		
		// Update master map
		master_map.remove(old_name);
		master_map.put(new_name, inst);
		return true;
	}
	
	public String toString()
	{
		return name;
	}
	
	/**
	 * Get a map of all the atomic instances
	 */
	public Hashtable<String, MasterInst> getAllAtomics()
	{
		Hashtable<String, MasterInst> atomic_map = new Hashtable<String, MasterInst>();
		getAllAtomics("", atomic_map);
		return atomic_map;		
	}

	/**
	 * Helper that adds all atomic instances to the given map
	 */
	private void getAllAtomics(String cur_hier, Hashtable<String, MasterInst> atomic_map)
	{
		Iterator<MasterInst> it = master_insts.iterator();
		while(it.hasNext())
		{
			MasterInst cur_inst = it.next();
			// If the current instance is atomic, add it to the map
			if (cur_inst.isAtomic())
				atomic_map.put(cur_hier + cur_inst.n, cur_inst);
			// If not tell it to add all of its atomics to the map
			else
				cur_inst.m.getAllAtomics(cur_hier + cur_inst.n + HIER_SEPARATOR, atomic_map);
		}		
	}
	
	public boolean isFiller() { return filler; }
	public boolean isAtomic() { return atomic; }
	public GridPoint getHeight() { return height;	}	
	public GridPoint getWidth() { return width; }	
	public void setName(String name) { this.name = name; }
	public String getName() { return name; }
	public MasterInst getLibInstance() { return lib_instance; }
	public void setLibInstance(MasterInst lib_instance) { this.lib_instance = lib_instance; }
	
	public Vector<MasterInst> getInstances() { return master_insts; }	
	public Hashtable<String, MasterInst> getInstanceMap() { return master_map; }

	public int compareTo(Master other_obj)
	{
		return name.compareTo(other_obj.name);		
	}

	/**
	 * Create a filler master for a master instance
	 */
	public static Master createFillerMaster(Master m, String master_name, Box fill_box, 
			GridPoint max_width, GridPoint max_height, double max_aspect_ratio)
	{
		FillerTree t = new FillerTree();
		t.calculateFill(fill_box, m);
		
		Master fill_master = new Master(master_name);
		Vector<MasterInst> fillers = t.getFillers(max_width, max_height, max_aspect_ratio);
		
		Iterator<MasterInst> it = fillers.iterator();
		while(it.hasNext())
			fill_master.addMasterInst(it.next());
		
		return fill_master;
	}
	
	/**
	 * Function that returns all atomic instances at their flattened coordinates
	 */
	public static List<MasterInst> getFlatInstances(Master m)
	{
		List<MasterInst> atomics = new LinkedList<MasterInst>();
		addFlatInstances(atomics, m, GridPoint.ZERO, GridPoint.ZERO, "");
		return atomics;
	}
	
	/**
	 * Helper function that adds atomics in a master hierarchically to an atomics list
	 */
	public static void addFlatInstances(List<MasterInst> atomics, Master m, GridPoint x_coord, GridPoint y_coord, String hier_name)
	{	
		// If it is an atomic, add it to the list of atomics
		if (m != null && m.isAtomic())
			atomics.add(new MasterInst(null, m, hier_name, x_coord, y_coord));
		// Otherwise recursively walk
		else if (m != null)
		{
			// Recursively add atomics through all sub instances
			Iterator<MasterInst> it = m.getInstances().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				// Have to do this for hierarhical name, or else you end up with an extra .
				String next_hier_name  = "";
				if(next_inst.isAtomic()) next_hier_name = hier_name + next_inst.n;
				else next_hier_name = hier_name + next_inst.n + HIER_SEPARATOR;					
				addFlatInstances(atomics, next_inst.m, GridPoint.add(x_coord, next_inst.x), GridPoint.add(y_coord, next_inst.y), next_hier_name);
			}
		}
		
	}
		
	/**
	 * Get the bounding box of a master instance
	 */
	public static Box getBoundingBox(Master m)
	{
		List<MasterInst> atomics = getFlatInstances(m);
		if (atomics.isEmpty())
			return new Box(0, 0, 0, 0);
		Box box = new Box(Double.MAX_VALUE, Double.MAX_VALUE, Double.MIN_VALUE, Double.MIN_VALUE);
		Iterator<MasterInst> it = atomics.iterator();
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			GridPoint.add(next_inst.x, next_inst.m.width);
			box.llx = GridPoint.min(box.llx, next_inst.x);
			box.lly = GridPoint.min(box.lly, next_inst.y);
			box.urx = GridPoint.max(box.urx, GridPoint.add(next_inst.x, next_inst.m.width));
			box.ury = GridPoint.max(box.ury, GridPoint.add(next_inst.y, next_inst.m.height));			
		}		
		return box;
	}
		
}

