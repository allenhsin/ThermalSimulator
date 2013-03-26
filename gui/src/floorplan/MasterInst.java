package floorplan;

import java.util.Enumeration;

import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * Simple class 
 *
 */
public class MasterInst implements MutableTreeNode
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

	@Override
	public void insert(MutableTreeNode child, int index)
	{ 
		m.getFloorplanInsts().insertElementAt((MasterInst) child, index);
	}

	public void remove(int index)
	{
		m.getFloorplanInsts().remove(index);		
	}

	public void remove(MutableTreeNode node)
	{
		m.getFloorplanInsts().remove((MasterInst) node);		
	}

	public void removeFromParent() { throw new Error("Should never happen"); }

	public void setParent(MutableTreeNode newParent) { throw new Error("Should never happen"); }

	public void setUserObject(Object object) { throw new Error("Should never happen"); }
}


