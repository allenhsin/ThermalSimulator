package display;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Paint;

import java.awt.AlphaComposite;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
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
	// Min/Max scale (essentially sets maximum zoom levels)
	public static final double MAX_SCALE = 100e9;
	public static final double MIN_SCALE = 1;
	
	// Some painting styles
	public static final Paint COLOR_BACKGROUND = Color.BLACK;
	public static final Paint COLOR_BORDER = Color.CYAN;
	public static final Paint COLOR_OUTLINE = Color.WHITE;
	public static final Paint COLOR_ATOMIC = Color.GRAY;
	public static final Paint COLOR_FILLER = Color.GRAY;
	public static final Paint COLOR_HIGHLIGHTS = Color.YELLOW;
	
	public static final float ALPHA_BACKGROUND = 1.0f;	
	public static final float ALPHA_BORDER = 1.0f;	
	public static final float ALPHA_OUTLINE = 1.0f;	
	public static final float ALPHA_ATOMIC = 0.6f;	
	public static final float ALPHA_FILLER = 0.3f;	
	public static final float ALPHA_HIGHLIGHTS = 0.5f;	

	// The master to render
	private Master render_target;
	// The temperature trace to follow
	private TemperatureTrace temp_trace;
	// Map of floorplan instance names to highlight
	private HashMap<String, Boolean> highlights;
	
	// Current rendered time step
	private int time;
	
	// Scaling factor
	private double scale;
	
	// Image offset
	private int offset_x;
	private int offset_y;

	// Image translation
	private double trans_x;
	private double trans_y;
	

	public FloorplanRender (Dimension image_size)
	{		
		setPreferredSize(image_size);

		// Highlights
		highlights = new HashMap<String, Boolean>();

		// Set time step to 0;
		time = 0;
		
		addComponentListener(new RenderComponentListener());
	}
	
	/**
	 * Set the floorplan to render
	 * @param floorplan
	 */
	public void setRenderTarget(Master render_target)
	{
		// Unload the temperature trace
		temp_trace = null;
		// Set this to be the new render target
		this.render_target = render_target;
		// Clear all highlights
		highlights.clear();
		// Go to default zoom if we are actually rendering something		
		if (render_target != null)
		{
			updateImageSize();
			zoom();
			repaint();
		}
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

		g.setPaintMode();
		// Fill background with some color
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_BACKGROUND));
		g.setPaint(COLOR_BACKGROUND);
		g.fillRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
		
		// If there is stuff to render
		if (render_target != null)
		{
			if (temp_trace != null)
			{
				paintTemperatures(g);
				paintOutlines(g, false);
			}
			else
				paintOutlines(g, true);
			paintHighlights(g);
		}
	}
	
	public synchronized void paintTemperatures(Graphics2D g)
	{
		// Paint temperatures helper
		paintTemperatures(g, render_target, new Coord(0.0, 0.0), "");
	}
	
	private synchronized void paintTemperatures(Graphics2D g, Master target, Coord origin, String hier_name)
	{
		if (target.isAtomic())
		{
			// Get maximum and minimum temperatures
			double max_temp = temp_trace.getMaxTemp();
			double min_temp = temp_trace.getMinTemp();
		
			// Get temperatures at this time step
			int idx = temp_trace.getIdxMap().get(hier_name);
			TemperatureStep temp_step = temp_trace.getTemperatureSteps()[time];
			
			// Set the painting color based on the temperature
			g.setPaint(TemperatureColor.getColor(temp_step.getTemperatures()[idx], max_temp, min_temp));
			FloorplanRectangle rect = new FloorplanRectangle(target, origin,
					trans_x, trans_y, offset_x, offset_y, scale);			
			g.fillRect(rect.x, rect.y, rect.w, rect.h);
		}
		else
		{
			// Recursively paint outlines through all sub instances
			Iterator<MasterInst> it = target.getInstances().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				// Kind of a hack to avoid the extra hierarchical separator
				if (next_inst.isAtomic())
					paintTemperatures(g, next_inst.m, new Coord(GridPoint.add(origin.x, next_inst.x), GridPoint.add(origin.y, next_inst.y)),
							hier_name + next_inst.n);					
				else
					paintTemperatures(g, next_inst.m, new Coord(GridPoint.add(origin.x, next_inst.x), GridPoint.add(origin.y, next_inst.y)),
							hier_name + next_inst.n + Master.HIER_SEPARATOR);
			}
		}
	}
	
	/**
	 * Draw all the outlines of blocks
	 */
	public synchronized void paintOutlines(Graphics2D g, boolean fill)
	{	
		// Paint outlines, starting with the top floorplan instance at origin 0, 0
		paintOutlines(g, fill, render_target, new Coord(0.0, 0.0));

		// Draw top-level border rectangle
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_BORDER));
		g.setPaint(COLOR_BORDER);
		FloorplanRectangle border = new FloorplanRectangle(Master.getBoundingBox(render_target), new Coord(0.0, 0.0),
				trans_x, trans_y, offset_x, offset_y, scale);
		g.drawRect(border.x,  border.y, border.w, border.h);
	}
	
	/**
	 * Paint a master instance at a specific location, returns the upper right
	 * coordinates
	 */
	private synchronized void paintOutlines(Graphics2D g, boolean fill, Master target, Coord origin)
	{
		// If it is a leaf, paint it
		if (target.isAtomic())
		{
			// Find the bounding rectangle
			FloorplanRectangle rect = new FloorplanRectangle(target, origin,
					trans_x, trans_y, offset_x, offset_y, scale);			
			// Fill the rectangle if we are asked to
			if (fill)
			{
				if (target.isFiller())
				{
					g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_FILLER));
					g.setPaint(COLOR_FILLER);		
					g.fillRect(rect.x, rect.y, rect.w, rect.h);				
				}
				else
				{
					g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_ATOMIC));
					g.setPaint(COLOR_ATOMIC);		
					g.fillRect(rect.x, rect.y, rect.w, rect.h);
				}
			}
			// Draw outline around rectangle
			g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_OUTLINE));
			g.setPaint(COLOR_OUTLINE);		
			g.drawRect(rect.x, rect.y, rect.w, rect.h);			
		}
		// Otherwise recursively call and paint
		else
		{
			// Recursively paint outlines through all sub instances
			Iterator<MasterInst> it = target.getInstances().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				paintOutlines(g, fill, next_inst.m, new Coord(GridPoint.add(origin.x, next_inst.x), GridPoint.add(origin.y, next_inst.y)));
			}
		}
	}

	public synchronized void paintHighlights(Graphics2D g)
	{
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_HIGHLIGHTS));
		g.setPaint(COLOR_HIGHLIGHTS);

		// Iterate through all sub-instances, see if any of them need to be highlighted
		Iterator<MasterInst> it = render_target.getInstances().iterator();		
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			if (highlights.containsKey(next_inst.n))
				paintHighlights(g, next_inst.m, new Coord(next_inst.x, next_inst.y));
		}
	}
	
	/**
	 * Helper for painting highlights
	 */
	private synchronized void paintHighlights(Graphics2D g, Master target, Coord origin)
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
			Iterator<MasterInst> it = target.getInstances().iterator();
			while(it.hasNext())
			{
				MasterInst next_inst = it.next();
				paintHighlights(g, next_inst.m, new Coord(GridPoint.add(origin.x, next_inst.x), GridPoint.add(origin.y, next_inst.y)));
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
	
	public void setScale(double scale)
	{
		this.scale = Math.min(Math.max(scale, MIN_SCALE), MAX_SCALE);
	}
	
	/** 
	 * Zooms the image by an amount
	 */
	public void zoom(double scale_factor)
	{
		setScale(scale * scale_factor);
	}

	// Centers the zoom
	public void zoom()
	{
		Box b_box = Master.getBoundingBox(render_target);
		// Set default scale	
		setScale(0.9 * Math.min(getSize().getWidth() / b_box.getWidth().toDouble(),
				getSize().getHeight() / b_box.getHeight().toDouble()));

		// Set translates
		trans_x = -b_box.llx.toDouble() - b_box.getWidth().toDouble() / 2;
		trans_y = -b_box.lly.toDouble() - b_box.getHeight().toDouble() / 2;
	}

	
	public void setTime(int time)
	{
		this.time = time;
	}
	
	public void updateImageSize()
	{ 
		offset_x = (int) getSize().getWidth() / 2;
		offset_y = (int) getSize().getHeight() / 2;
	}
	
	/**
	 * Translates a image pixel coordinate to a grid point coordinate
	 */
	public Coord pixelToCoord(int x, int y)
	{
		int x_off = x - offset_x;
		int y_off = ((int) getSize().getHeight() - y) - offset_y;
		double x_cd = x_off / scale;
		double y_cd = y_off / scale;
		
		return new Coord(x_cd - trans_x, y_cd - trans_y);
	}
	
	public Master getRenderTarget() { return render_target; }
	public TemperatureTrace getTemperatureTrace() { return temp_trace; }	
}

class RenderComponentListener implements ComponentListener
{
	public void componentHidden(ComponentEvent e) {}
	public void componentMoved(ComponentEvent e) {}
	public void componentResized(ComponentEvent e) 
	{
		((FloorplanRender) e.getSource()).updateImageSize();	
		((FloorplanRender) e.getSource()).repaint();
	}
	public void componentShown(ComponentEvent e) {}	
}
