package floorplan;

import java.util.Iterator;
import java.util.Stack;
import java.util.Vector;

import display.FloorplanRender;

/**
 * Class responsible for populating itself to figure out where to place leaf filler
 * instances
 */
public class FillerTree 
{
	// Root filler tree node
	private FillerTreeNode root;
	
	// Calculate the fill for a master
	public void calculateFill(Master m)
	{
		Box b_box = Master.getBoundingBox(m);
		root = new FillerTreeNode(b_box.llx, b_box.lly, b_box.urx, b_box.ury);	
		calculateFill(m, new GridPoint(0), new GridPoint(0));
	}
	
	private void calculateFill(Master m, GridPoint x_coord, GridPoint y_coord)
	{
		// If it is a leaf, subtract it from the fill shape
		if (m.isAtomic())
			subtractFill(m, x_coord, y_coord);
		// Otherwise recursively walk through and calculate fill
		else
		{
			// Recursively paint outlines through all sub instances
			Iterator<MasterInst> it = m.getInstances().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				calculateFill(next_inst.m, GridPoint.add(x_coord, next_inst.x), GridPoint.add(y_coord, next_inst.y));
			}
		}
		
	}
	
	// Get a list of filler instance for a master
	public Vector<MasterInst> getFillers()
	{
		return root.getFillers();
	}
	
	// Tell the filler tree node to subtract fill
	private void subtractFill(Master m, GridPoint x_coord, GridPoint y_coord)
	{
		// Make sure the master to subtract is atomic
		if(!m.isAtomic())
			throw new Error("Error: expected atomic master in filler subroutine, got non-atomic master.");
		// Tell the root to subtract the fill
		root.subtractFill(m, x_coord, y_coord);
	}
}

/**
 * Class representing a node
 */
class FillerTreeNode
{
	// Sub-nodes populated once the tree node is no longer a leaf
	// North node contains node(s) whose Y-coordinates are bigger than the upper-right Y-coordinate
	private FillerTreeNode n;
	// South node contains node(s) whose Y-coordinates are smaller than the lower-left Y-coordinate
	private FillerTreeNode s;
	// East node contains node(s) whose X-coordinates are bigger than the upper-right X-coordinate
	// and Y-coordinates between the upper-right and lower-left Y-coordinate
	private FillerTreeNode e;
	// West node contains node(s) whose X-coordinates are smaller than the lower-left X-coordinate
	// and Y-coordinates between the upper-right and lower-left Y-coordinate
	private FillerTreeNode w;
	
	// A leaf tree node essentially becomes a filler instance
	boolean leaf;
	// Whether this node has zero area
	boolean zero_area;
	
	// Lower-left and upper-right coordinates
	GridPoint llx, lly, urx, ury;
	
	// Create tree node
	FillerTreeNode(GridPoint llx, GridPoint lly, GridPoint urx, GridPoint ury)
	{
		this.llx = llx;
		this.lly = lly;
		this.urx = urx;
		this.ury = ury;
		this.leaf = true;
		
		// Check if this filler has zero area
		this.zero_area = GridPoint.greaterThanOrEqualsTo(llx, urx) || 
				GridPoint.greaterThanOrEqualsTo(lly, ury); 
	}
	
	// Tell the filler tree node to subtract fill
	void subtractFill(Master m, GridPoint x_coord, GridPoint y_coord)
	{
		// Calculate the bounding box of the subtracted master, with limits
		// at the bounding box of the current node
		GridPoint m_llx = GridPoint.max(x_coord, llx);
		GridPoint m_lly = GridPoint.max(y_coord, lly);
		GridPoint m_urx = GridPoint.min(GridPoint.add(x_coord, m.getWidth()), urx);
		GridPoint m_ury = GridPoint.min(GridPoint.add(y_coord, m.getHeight()), ury);
		
		// First, decide if I need to do anything
		// Only need to do things if the size of this filler node is non-zero
		if (!zero_area)
		{
			// If the following isn't true, then the instance falls outside the node's boundaries and
			// I don't need to do anything
			if (GridPoint.greaterThanOrEqualsTo(m_urx, llx) && GridPoint.lessThanOrEqualsTo(m_llx, urx) && 
					GridPoint.greaterThanOrEqualsTo(m_ury, lly) && GridPoint.lessThanOrEqualsTo(m_lly, ury))
			{
				// If this is a leaf node...split and make sub leafs
				if(leaf)
				{
					// This is no longer a leaf
					leaf = false;
					// Create new sub leaf nodes
					n = new FillerTreeNode(llx, m_ury, urx, ury);
					s = new FillerTreeNode(llx, lly, urx, m_lly);
					e = new FillerTreeNode(m_urx, m_lly, urx, m_ury);
					w = new FillerTreeNode(llx, m_lly, m_llx, m_ury);
				}
				// If not, then perform the subtractFill operation on all the children
				else
				{
					n.subtractFill(m, x_coord, y_coord);
					s.subtractFill(m, x_coord, y_coord);
					e.subtractFill(m, x_coord, y_coord);
					w.subtractFill(m, x_coord, y_coord);
				}
			}
		}
	}
	
	// Get a vector of all the fillers
	public Vector<MasterInst> getFillers()
	{
		return addFillers(this);
	}
	
	// Add all the fillers that are hierarchically below this filler tree
	private static Vector<MasterInst> addFillers(FillerTreeNode root)
	{
		Vector<MasterInst> fillers = new Vector<MasterInst>();
		// Maintain a stack
		Stack<FillerTreeNode> stack = new Stack<FillerTreeNode>();
		stack.push(root);
		
		// Keep going until stack is empty
		while(!stack.isEmpty())
		{
			FillerTreeNode cur_node = stack.pop();
			// Only need to care if this has non-zero area
			if (!cur_node.zero_area)
			{
				if (cur_node.leaf)
				{
					GridPoint width = GridPoint.sub(cur_node.urx, cur_node.llx);
					GridPoint height = GridPoint.sub(cur_node.ury, cur_node.lly);
					// Only need to add it if it is a finitely big fill shape
					if (!GridPoint.equals(width, GridPoint.ZERO) && 
							!GridPoint.equals(height,  GridPoint.ZERO))
					{
						fillers.add(new MasterInst(null, new Master(width, height), 
								"Fill_" + fillers.size(), cur_node.llx, cur_node.lly));
					}
				}
				else
				{
					stack.push(cur_node.n);
					stack.push(cur_node.s);
					stack.push(cur_node.e);
					stack.push(cur_node.w);
				}			
			}			
		}
		return fillers;
	}
}

