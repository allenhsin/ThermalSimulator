package floorplan;

import java.util.Enumeration;

import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * Simple class 
 *
 */
public class MasterInst implements TreeNode, Comparable<MasterInst>
{
	// Parent master of this instance
	public Master parent;
	// This is an instance of the master
	public Master m;
	// Instance name
	public String n;
	// Instance location (x, y)
	public GridPoint x, y;
	
	public MasterInst(Master parent, Master m, String n, GridPoint x, GridPoint y)
	{
		this.parent = parent;
		this.m = m;
		this.n = n;
		this.x = x;
		this.y = y;		
	}
	
	public String toString()
	{
		return n + " : " + m.getName();
	}
	
	public boolean isFiller()
	{
		return m.isFiller();
	}
	
	public boolean isAtomic()
	{
		return m.isAtomic();
	}
	
	/** Methods that implement TreeNode functionality */
	public Enumeration<MasterInst> children() { return m.getInstances().elements(); }
	public boolean getAllowsChildren() { return true; }
	public TreeNode getChildAt(int idx) { return m.getInstances().get(idx); }
	public int getChildCount() { return m.getInstances().size(); }
	public int getIndex(TreeNode node) { return m.getInstances().indexOf(node); }
	public TreeNode getParent() { return null; }
	public boolean isLeaf() { return m.isAtomic(); }
	
	public int compareTo(MasterInst a) 
	{
		double my_width = m.getWidth().toDouble();
		double my_height = m.getHeight().toDouble();
		double a_width = a.m.getWidth().toDouble();
		double a_height = a.m.getHeight().toDouble();
		
		if ((my_width * my_height) == (a_width * a_height)) return 0;
		else if ((my_width * my_height) > (a_width * a_height)) return -1;
		else return 1;
	}
}


