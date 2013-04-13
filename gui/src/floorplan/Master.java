package floorplan;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
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
	
	// Constructor for a leaf master
	public Master(GridPoint width, GridPoint height)
	{
		this("Atomic");
		atomic = true;
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
	 * Renames a master
	 */
	public boolean renameMaster(String old_name, String new_name)
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
	

	/**
	 * Get the bounding box of a master instance
	 */
	public static Box getBoundingBox(Master m)
	{
		return getBoundingBox(m, new Coord(0, 0));
	}
	
	private static Box getBoundingBox(Master m, Coord origin)
	{
		if (m.isAtomic())
			return new Box(origin.x, origin.y,
					GridPoint.add(origin.x, m.getWidth()), GridPoint.add(origin.y, m.getHeight()));
		
		// Create new box instance
		Box box = new Box(Double.MAX_VALUE, Double.MAX_VALUE, Double.MIN_VALUE, Double.MIN_VALUE);
		// Iterate through all elements
		Iterator<MasterInst> it = m.getInstances().iterator();
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			Box next_box = getBoundingBox(next_inst.m, new Coord(GridPoint.add(origin.x, next_inst.x), 
					GridPoint.add(origin.y, next_inst.y)));
			box.llx = GridPoint.min(next_box.llx, box.llx);
			box.lly = GridPoint.min(next_box.lly, box.lly);
			box.urx = GridPoint.max(next_box.urx, box.urx);
			box.ury = GridPoint.max(next_box.ury, box.ury);
		}
		return box;
	}
	
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
	
}

