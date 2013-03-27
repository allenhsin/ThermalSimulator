package display;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Stroke;

import java.awt.AlphaComposite;
import java.util.HashMap;
import java.util.Iterator;

import javax.swing.JComponent;
import floorplan.*;
import temperature.*;

/** 
 * The floorplan render class is responsible for rendering a master instance in various forms
 * as needed by the top-level tool
 */
public class FloorplanRender extends JComponent
{	
	// The master instance to render
	private MasterInst render_target;
	// The temperature trace to follow
	private TemperatureTrace temp_trace;
	// Map of floorplan instance names to highlight
	private HashMap<String, Boolean> highlights;
	
	// Current rendered time step
	private int time;
	
	// Scaling factor
	private double scale;
	
	// Image offset
	private double offset_x;
	private double offset_y;

	// Image translation
	private double trans_x;
	private double trans_y;

	public FloorplanRender (Dimension image_size)
	{		
		//setSize(image_size);
		setPreferredSize(image_size);
		
		offset_x = image_size.getWidth() / 2;
		offset_y = image_size.getHeight() / 2;
		
		// Highlights
		highlights = new HashMap<String, Boolean>();

		// Set time step to 0;
		time = 0;
		
	}
	
	/**
	 * Set the floorplan to render
	 * @param floorplan
	 */
	public void setRenderTarget(MasterInst render_target)
	{
		// Unload the temperature trace
		temp_trace = null;
		// Set this to be the new render target
		this.render_target = render_target;
		// Clear all highlights
		highlights.clear();
		// Default zoom
		zoom();
		repaint();
	}
	
	/**
	 * Set the temperature trace to display
	 * @param temp_trace
	 */
	public void setTempTrace(TemperatureTrace temp_trace)
	{
		this.temp_trace = temp_trace;
		repaint();
	}
	
	public synchronized void paintComponent (Graphics g0) 
	{
		super.paintComponent(g0);
		Graphics2D g = (Graphics2D) g0;
		g.scale(1, -1);
		g.translate(0, -getHeight());		

		g.setPaintMode ();
		g.setColor(Color.black);
		// Fill background with black
		g.fillRect(0, 0, (int) getPreferredSize().getWidth(), (int) getPreferredSize().getHeight());
		
		// If there is stuff to render
		if (render_target != null)
		{
			if (temp_trace != null)
				paintTemperatures(g);
			paintOutlines(g);
			paintHighlights(g);
		}
	}
	
	public synchronized void paintTemperatures(Graphics2D g)
	{
		// Disabled until new temperature format is done
//		// Get list of floorplan instances
//		Vector<Floorplan> fp_insts = floorplan.getChildren();
//		
//		// Get maximum and minimum temperatures
//		double max_temp = temp_trace.getMaxTemp();
//		double min_temp = temp_trace.getMinTemp();
//		
//		// Get temperatures at this time step
//		TemperatureStep temp_step = temp_trace.getTemperatureSteps()[time];
//
//		// Iterate through all rectangles
//		Iterator<Floorplan> it = fp_insts.iterator();
//		while(it.hasNext())
//		{
//			// Get the current floorplan
//			Floorplan f = it.next();
//			int idx = temp_trace.getIdxMap().get(f.getName());
//			double temperature = temp_step.getTemperatures()[idx];
//			
//			// Set the painting color based on the temperature
//			g.setColor(TemperatureColor.getColor(temperature, max_temp, min_temp));
//			
//			// Fill the rectangle with the correct color
//			FloorplanRectangle rect = new FloorplanRectangle(f, floorplan,
//					trans_x, trans_y, offset_x, offset_y, scale);			
//			g.fillRect(rect.x, rect.y, rect.w, rect.h);
//		}
	}
	
	/**
	 * Draw all the outlines of blocks
	 */
	public synchronized void paintOutlines(Graphics2D g)
	{	
		// Draw block outlines in white
		g.setColor(Color.white);		
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 1.0));
		// Paint outlines, starting with the top floorplan instance at origin 0, 0
		paintOutlines(g, render_target, new Coord(0.0, 0.0));

		// Draw top-level border rectangle in cyan
		g.setColor(Color.cyan);
		FloorplanRectangle border = new FloorplanRectangle(getBoundingBox(render_target), new Coord(0.0, 0.0),
				trans_x, trans_y, offset_x, offset_y, scale);
		g.drawRect(border.x,  border.y, border.w, border.h);
	}
	
	/**
	 * Paint a master instance at a specific location, returns the upper right
	 * coordinates
	 */
	private synchronized void paintOutlines(Graphics2D g, MasterInst target, Coord origin)
	{
		// If it is a leaf, paint it
		if (target.isAtomic())
		{
			FloorplanRectangle rect = new FloorplanRectangle(target, origin,
					trans_x, trans_y, offset_x, offset_y, scale);			
			g.drawRect(rect.x, rect.y, rect.w, rect.h);
		}
		// Otherwise recursively call and paint
		else
		{
			// Recursively paint outlines through all sub instances
			Iterator<MasterInst> it = target.m.getFloorplanInsts().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				paintOutlines(g, next_inst, new Coord(origin.x + next_inst.x, origin.y + next_inst.y));
			}
		}
	}

	public synchronized void paintHighlights(Graphics2D g)
	{
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 0.5));
		g.setColor(Color.white);

		// Iterate through all sub-instances, see if any of them need to be highlighted
		Iterator<MasterInst> it = render_target.m.getFloorplanInsts().iterator();		
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			if (highlights.containsKey(next_inst.n))
				paintHighlights(g, next_inst, new Coord(next_inst.x, next_inst.y));
		}
	}
	
	/**
	 * Helper for painting highlights
	 */
	private synchronized void paintHighlights(Graphics2D g, MasterInst target, Coord origin)
	{
		// If it is a leaf, paint it
		if (target.isAtomic())
		{
			FloorplanRectangle rect = new FloorplanRectangle(target, origin,
					trans_x, trans_y, offset_x, offset_y, scale);			
			g.fillRect(rect.x, rect.y, rect.w, rect.h);
		}
		// Otherwise recursively call and paint
		else
		{
			// Recursively paint outlines through all sub instances
			Iterator<MasterInst> it = target.m.getFloorplanInsts().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				paintHighlights(g, next_inst, new Coord(origin.x + next_inst.x, origin.y + next_inst.y));
			}
		}
	}
	
	public HashMap<String, Boolean> getHighlights()
	{
		return highlights;
	}
	
	// Moves the image
	public void translate(int x, int y)
	{
		trans_x += x / scale;
		trans_y += y / scale;
	}
	
	// Zooms the image by an amount
	public void zoom(double scale_factor)
	{
		scale = scale * scale_factor;
	}

	// Centers the zoom
	public void zoom()
	{
		Box b_box = getBoundingBox(render_target);
		// Set default scale	
		scale = 0.9 * Math.min(getPreferredSize().getWidth() / b_box.getWidth(),
				getPreferredSize().getHeight() / b_box.getHeight());
		
		// Set translates
		trans_x = -b_box.getWidth() / 2;
		trans_y = -b_box.getHeight() / 2;
	}

	/**
	 * Get the bounding box of a master instance
	 */
	public static Box getBoundingBox(MasterInst m)
	{
		return getBoundingBox(m, new Coord(0, 0));
	}
	
	private static Box getBoundingBox(MasterInst m, Coord origin)
	{
		if (m.isAtomic())
			return new Box(origin.x, origin.y,
					origin.x + m.m.getWidth(), origin.y + m.m.getHeight());
		
		// Create new box instance
		Box box = new Box(Double.MAX_VALUE, Double.MAX_VALUE, Double.MIN_VALUE, Double.MIN_VALUE);
		// Iterate through all elements
		Iterator<MasterInst> it = m.m.getFloorplanInsts().iterator();
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			Box next_box = getBoundingBox(next_inst, new Coord(origin.x + next_inst.x, origin.y + next_inst.y));
			box.llx = Math.min(next_box.llx, box.llx);
			box.lly = Math.min(next_box.lly, box.lly);
			box.urx = Math.max(next_box.urx, box.urx);
			box.ury = Math.max(next_box.ury, box.ury);
		}
		return box;
	}
	
	public void setTime(int time)
	{
		this.time = time;
	}
	
	public MasterInst getRenderTarget() { return render_target; }
	public TemperatureTrace getTemperatureTrace() { return temp_trace; }	
}

/**
 * Simple class with X, Y double coordinates
 */
class Coord
{
	double x, y;
	Coord(double x, double y)
	{ 
		this.x = x; 
		this.y = y; 
	}
}

/**
 * Simple class that represents a box
 */
class Box
{
	double llx, lly, urx, ury;
	Box(double llx, double lly, double urx, double ury)
	{
		this.llx = llx;
		this.lly = lly;
		this.urx = urx;
		this.ury = ury;
	}
	
	double getHeight()
	{
		return ury - lly;
	}
	double getWidth()
	{
		return urx - llx;
	}
}
