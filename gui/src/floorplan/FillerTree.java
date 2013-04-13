package floorplan;

import java.util.Iterator;
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
		root = new FillerTreeNode(0, 0, 10e-3, 10e-3);	
		calculateFill(m, 0, 0);
	}
	
	private void calculateFill(Master m, double x_coord, double y_coord)
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
				calculateFill(next_inst.m, x_coord + next_inst.x, y_coord + next_inst.y);
			}
		}
		
	}
	
	// Get a list of filler instance for a master
	public Vector<MasterInst> getFillers()
	{
		return root.getFillers();
	}
	
	// Tell the filler tree node to subtract fill
	private void subtractFill(Master m, double x_coord, double y_coord)
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
	
	// Lower-left and upper-right coordinates
	double llx, lly, urx, ury;
	
	// Create tree node
	FillerTreeNode(double llx, double lly, double urx, double ury)
	{
		this.llx = llx;
		this.lly = lly;
		this.urx = urx;
		this.ury = ury;
		this.leaf = true;
	}
	
	// Tell the filler tree node to subtract fill
	void subtractFill(Master m, double x_coord, double y_coord)
	{
		// Calculate the bounding box of the subtracted master, with limits
		// at the bounding box of the current node
		double m_llx = Math.max(x_coord, llx);
		double m_lly = Math.max(y_coord, lly);
		double m_urx = Math.min(x_coord + m.getWidth(), urx);
		double m_ury = Math.min(y_coord + m.getHeight(), ury);
		
		// First, decide if I need to do anything. If the following isn't true, then the instance
		// falls outside the node's boundaries and I don't need to do anything
		if (m_urx >= llx && m_llx <= urx && 
				m_ury >= lly && m_lly <= ury)
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
	
	// Get a vector of all the fillers
	public Vector<MasterInst> getFillers()
	{
		Vector<MasterInst> to_return = new Vector<MasterInst>();
		addFillers(to_return);
		return to_return;
	}
	
	// Add all the fillers that are hierarchically below this filler tree
	private void addFillers(Vector<MasterInst> fillers)
	{
		if (leaf)
		{
			double width = urx - llx;
			double height = ury - lly;
			// Only need to add it if it is a finitely big fill shape
			// HACK: This implicitly assumes that the grid size is 1nm
			if (width >= 1e-9 && height >= 1e-9)
			{
				fillers.add(new MasterInst(null, new Master(width, height), 
						"Fill_" + fillers.size(), llx, lly));
			}
		}
		else
		{
			n.addFillers(fillers);
			s.addFillers(fillers);
			e.addFillers(fillers);
			w.addFillers(fillers);
		}
	}
}

