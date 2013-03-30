package floorplan;

import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Vector;

import javax.swing.ComboBoxModel;
import javax.swing.event.ListDataListener;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * A master map holds all the floorplan masters in a map format
 * @author DrunkenMan
 *
 */
public class MasterMap
{
//	
	// Hashtable containing all the masters
	private Hashtable<String, Master> masters;
	// Vector containing all the masters, each instantiated as their own master
	// library instance
	private Vector<MasterInst> lib_insts;
	// The selected master
	private Master selected;
	// The current unnamed number count
	private int new_master_count;

	public MasterMap()
	{
		masters = new Hashtable<String, Master>();
		lib_insts = new Vector<MasterInst>();
		new_master_count = 0;
	}	
	
	/**
	 * Searches for recursive masters within this master
	 */
	public boolean hasRecursiveMasters(Master master)
	{
		return hasRecursiveMasters(master, new HashMap<Master, Boolean>());
	}

	/**
	 * Searches for recursive masters globally
	 */
	public boolean hasRecursiveMasters()
	{
		// Check all masters to see if any of them are recursive
		Iterator<Master> it = masters.values().iterator();
		while(it.hasNext())
		{
			if (hasRecursiveMasters(it.next()))
				return true;
		}		
		return false;
	}
	
	/**
	 * Search for recursive masters helper
	 */
	private boolean hasRecursiveMasters(Master master, HashMap<Master, Boolean> parent_masters)
	{
		// If I find a master that is already in the list of parent masters, then
		// there is recursion
		if (parent_masters.containsKey(master))
			return true;
		
		// Put myself in the list of parent masters
		parent_masters.put(master, true);
		Iterator<MasterInst> it = master.getInstances().iterator();
		while (it.hasNext())
		{
			if (hasRecursiveMasters(it.next().m, parent_masters))
				return true;
		}		
		// Remove myself from the list of parent masters
		parent_masters.remove(master);
		return false;
	}

	/**
	 * Add a new, empty, unnamed master to the map
	 */
	public void addMaster() throws Exception
	{
		addMaster(new Master("Unnamed_" + (new_master_count++)));
	}
	
	/**
	 * Add a new master to the map
	 */
	public void addMaster(Master m) throws Exception
	{
		if (hasMaster(m.getName()))
			throw new Exception("Duplicate master: " + m.getName());

		// Create new library instance and let the master know about it
		MasterInst lib_instance = new MasterInst(null, m, "(Lib)", 0.0, 0.0);
		m.setLibInstance(lib_instance);
		// Add the master and lib instance
		masters.put(m.getName(), m);		
		lib_insts.add(lib_instance);
	}
	
	/**
	 * Removes a master, will check if the master is used as an
	 * instance in another master and throw an exception if so
	 */
	public void removeMaster(Master m) throws Exception
	{
		removeMaster(m.getName(), true);
	}

	/**
	 * Removes a master by name, will check if the master is used as an
	 * instance in another master and throw an exception if so
	 */
	public void removeMaster(String name) throws Exception
	{
		removeMaster(name, true);
	}

	/**
	 * Returns the other masters that instantiates the master
	 */
	public Vector<Master> isInstantiated(Master m)
	{
		Vector<Master> users = new Vector<Master>();
		// Iterate through all masters
		Iterator<Master> it = masters.values().iterator();
		while(it.hasNext())
		{
			Master cur_master = it.next();
			// Check for whether this instance exists in all instances
			for (int i = 0; i < cur_master.getInstances().size(); ++i)
			{
				if (cur_master.getInstances().get(i).m == m)
				{
					users.add(cur_master);
					break;
				}
			}
		}
		return users;
	}
	
	/**
	 * Private removes a master helper, boolean to check if it checks for
	 * usage in other masters
	 */
	private void removeMaster(String name, boolean check) throws Exception
	{
		// Check to make sure the master exists
		if (!hasMaster(name))
			throw new Exception("Master does not exist: " + name);

		// Get the master
		Master m = masters.get(name);

		// Get the list of users for this master anywhere
		Vector<Master> users = isInstantiated(m);
		if (check && !users.isEmpty())
			throw new Exception("Master '" + name + "' is instantiated in another master: " +
					users.firstElement().toString());
		
		// remove it from the map
		masters.remove(name);		
		// Find it in the list of library instances
		Iterator<MasterInst> it = lib_insts.iterator();
		int idx = 0;
		int remove_idx = -1;
		while(it.hasNext())
		{
			if (it.next().m == m) remove_idx = idx;
			++idx;
		}
		// Delete it
		lib_insts.remove(remove_idx);
	}
	
	/**
	 * Get a new master from the map
	 */
	public Master getMaster(String name) throws Exception
	{
		if (hasMaster(name))
			return masters.get(name);		
		throw new Exception("Instance master does not exist: " + name);
	}
	
	/**
	 * Has a master by that already been defined?
	 */
	public boolean hasMaster(String name)
	{
		return masters.containsKey(name);
	}
	
	/**
	 * Renames a master in the map
	 */
	public void renameMaster(String old_name, String new_name) throws Exception
	{
		if (hasMaster(new_name))
			throw new Exception("Instance master with new name already exists: " + new_name);
		
		Master m = getMaster(old_name);
		m.setName(new_name);
		// Don't need to check
		removeMaster(old_name, false);
		addMaster(m);
	}
	
	/**
	 * Get a master
	 */
	public Master getDefaultMaster()
	{
		if (lib_insts.isEmpty())
			return null;
		return lib_insts.lastElement().m;
	}
	
	/**
	 * Returns a nice sorted array of all the masters
	 */
	public Master[] toArray()
	{
		Master[] arr_masters = masters.values().toArray(new Master[0]);
		Arrays.sort(arr_masters);
		return arr_masters;
	}
	
	public Hashtable<String, Master> getMasters() { return masters; }
	
}

