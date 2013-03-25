package floorplan;

import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;

import javax.swing.tree.TreeNode;

public class MasterMap extends Hashtable<String, Master>
{
	// The master instance designated as the top-level master instance
	private MasterInst top_inst;

	public void setTop(MasterInst top_inst)
	{
		this.top_inst = top_inst;
	}
	
	public MasterInst getTop()
	{
		return top_inst;
	}

}
