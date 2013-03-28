package floorplan;

import java.io.File;
import java.io.IOException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ComboBoxModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.MutableComboBoxModel;
import javax.swing.event.ListDataListener;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * A master map holds all the floorplan masters in a map format
 * @author DrunkenMan
 *
 */
public class MasterMap implements MutableComboBoxModel<Master>,MutableTreeNode
{
//	
	// Hashtable containing all the masters
	public Hashtable<String, Master> masters;
	// Vector containing all the masters, each instantiated as their own master
	// library instance
	Vector<MasterInst> lib_insts;
	// Default instance to load
	private Master default_master;
	// The selected master
	private Master selected;
	// The current unnamed number count
	private int new_master_count;

	public MasterMap()
	{
		masters = new Hashtable<String, Master>();
		lib_insts = new Vector<MasterInst>();
		default_master = new Master("Unnamed_0");
		new_master_count = 1;
	}	
	
	public boolean hasRecursiveMasters(Master master)
	{
		return hasRecursiveMasters(master, new HashMap<Master, Boolean>());
	}

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
	
	private boolean hasRecursiveMasters(Master master, HashMap<Master, Boolean> parent_masters)
	{
		// If I find a master that is already in the list of parent masters, then
		// there is recursion
		if (parent_masters.containsKey(master))
			return true;
		
		// Put myself in the list of parent masters
		parent_masters.put(master, true);
		Iterator<MasterInst> it = master.getFloorplanInsts().iterator();
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
		MasterInst lib_instance = new MasterInst(m, "(Lib)", 0.0, 0.0);
		m.setLibInstance(lib_instance);
		// Add the master and lib instance
		masters.put(m.getName(), m);		
		lib_insts.add(lib_instance);
		// Update the default master to this one
		default_master = m; 
	}
	
	public void removeMaster(String name) throws Exception
	{
		// Check to make sure the master exists
		if (!hasMaster(name))
			throw new Exception("Master does not exist: " + name);

		// Get the master
		Master m = masters.get(name);
		// remove it fromt he map
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
		removeMaster(old_name);
		addMaster(m);
	}
	
	/**
	 * Get the default selected instance
	 */
	public Master getDefaultMaster()
	{
		return default_master;
	}
	
	/** 
	 * Methods that implement ComboBoxModel functionality 
	 * The combo box methods must return objects of type Master
	 */
	
	public void addListDataListener(ListDataListener arg0) {}
	public Master getElementAt(int idx) { return lib_insts.get(idx).m; }
	public int getSize() { return lib_insts.size(); }
	public void removeListDataListener(ListDataListener arg0) {}
	public Object getSelectedItem() { return selected; }
	public void setSelectedItem(Object item) { selected = (Master) item; }

	public void addElement(Master item)
	{
		System.out.println("BLAH");
		try {addMaster((Master) item);} catch (Exception e) {};
	}
	
	public void insertElementAt(Master item, int idx)
	{
		System.out.println("BLAH2");
		try {addMaster((Master) item);} catch (Exception e) {};
	}
	/** End ComboBoxModel methods */
	
	/** Methods that implement TreeNode functionality
	 * The TreeNode methods must return objects of type MasterInst */
	public Enumeration<MasterInst> children() { return lib_insts.elements(); }	
	public boolean getAllowsChildren() { return true; }	
	public TreeNode getChildAt(int idx) { return lib_insts.get(idx); }	
	public int getChildCount() { return lib_insts.size(); }	
	public int getIndex(TreeNode node) { return lib_insts.indexOf(node); }	
	public TreeNode getParent() { return null; }	
	public boolean isLeaf() { return lib_insts.isEmpty(); }

	// Should never happen
	public void insert(MutableTreeNode node, int idx) { throw new Error("Should never happen"); }
	public void remove(int arg0) { throw new Error("Should never happen"); }
	public void remove(MutableTreeNode arg0) { throw new Error("Should never happen"); }
	public void removeFromParent() { throw new Error("Should never happen"); }
	public void setParent(MutableTreeNode arg0) { throw new Error("Should never happen"); }
	public void setUserObject(Object arg0) { throw new Error("Should never happen"); }

	@Override
	public void removeElement(Object arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void removeElementAt(int arg0) {
		// TODO Auto-generated method stub
		
	}
	
	/** End TreeNode methods */	
}

