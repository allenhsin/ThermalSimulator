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
	// Parent master of this instance
	public Master parent;
	// This is an instance of the master
	public Master m;
	// Instance name
	public String n;
	// Instance location (x, y)
	public double x, y;
	
	public MasterInst(Master parent, Master m, String n, double x, double y)
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

	@Override
	public void insert(MutableTreeNode child, int index)
	{ 
		m.getInstances().insertElementAt((MasterInst) child, index);
	}

	public void remove(int index)
	{
		m.getInstances().remove(index);		
	}

	public void remove(MutableTreeNode node)
	{
		m.getInstances().remove((MasterInst) node);		
	}

	public void removeFromParent() { throw new Error("Should never happen"); }

	public void setParent(MutableTreeNode newParent) { throw new Error("Should never happen"); }

	public void setUserObject(Object object) { throw new Error("Should never happen"); }
}


