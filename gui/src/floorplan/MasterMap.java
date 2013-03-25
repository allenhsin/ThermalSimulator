package floorplan;

import java.io.File;
import java.io.IOException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ComboBoxModel;
import javax.swing.event.ListDataListener;
import javax.swing.tree.TreeNode;

public class MasterMap extends Hashtable<String, Master> implements ComboBoxModel
{
	// The master instance designated as the top-level master instance
	private MasterInst top_inst;
	
	// The selected master
	private Master selected;

	public void setTop(MasterInst top_inst)
	{
		this.top_inst = top_inst;
	}
	
	public MasterInst getTop()
	{
		return top_inst;
	}
	
	public boolean hasRecursiveMasters()
	{
		return hasRecursiveMasters(getTop().m, new HashMap<Master, Boolean>());
	}
	
	public boolean hasRecursiveMasters(Master master, HashMap<Master, Boolean> parent_masters)
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
	
	public void addListDataListener(ListDataListener arg0) {}
	
	public Object getElementAt(int idx)
	{
		Enumeration<Master> enumerator = elements();
		if (idx >= size())
			return null;

		for (int i = 0; i < idx; ++i)
			enumerator.nextElement();
		
		return enumerator.nextElement();
	}

	public int getSize() { return size(); }

	public void removeListDataListener(ListDataListener arg0) {}

	public Object getSelectedItem() { return selected; }

	public void setSelectedItem(Object item) { selected = (Master) item; }

}
