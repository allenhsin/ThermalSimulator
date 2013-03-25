package floorplan;

import java.util.Enumeration;

import javax.swing.tree.TreeNode;

/**
 * Simple class 
 *
 */
public class MasterInst implements TreeNode
{
	// Instance of the master
	public Master m;
	// Instance name
	public String n;
	// Instance location (x, y)
	public double x, y;
	
	public MasterInst(Master m, String n, double x, double y)
	{
		this.m = m;
		this.n = n;
		this.x = x;
		this.y = y;		
	}
	
	public String toString()
	{
		return n + " : " + m.getName();
	}
	
	/** Methods that implement TreeNode functionality */
	public Enumeration children() { return m.getFloorplanInsts().elements(); }
	public boolean getAllowsChildren() { return true; }
	public TreeNode getChildAt(int idx) { return m.getFloorplanInsts().get(idx); }
	public int getChildCount() { return m.getFloorplanInsts().size(); }
	public int getIndex(TreeNode node) { return m.getFloorplanInsts().indexOf(node); }
	public TreeNode getParent() { return null; }
	public boolean isLeaf() { return (m.getFloorplanInsts().size() == 0); }
}


