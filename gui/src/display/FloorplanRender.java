package display;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Stroke;

import java.awt.AlphaComposite;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.geom.AffineTransform;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

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
	public static final Paint COLOR_TEMPTEXT = Color.BLACK;
	
	public static final float ALPHA_BACKGROUND = 1.0f;	
	public static final float ALPHA_BORDER = 1.0f;	
	public static final float ALPHA_OUTLINE = 1.0f;	
	public static final float ALPHA_ATOMIC = 0.6f;	
	public static final float ALPHA_FILLER = 0.3f;	
	public static final float ALPHA_HIGHLIGHTS = 1.0f;
	
	public static final float LINEWIDTH_HIGHLIGHTS = 4f;
	
	public static final Font FONT_TEMPTEXT = new Font("Arial", Font.BOLD, 10);
	public static final String FORMAT_TEMPTEXT = "%6.3f";
	
	
	// The master to render
	private Master render_target;
	// The temperature trace to follow
	private TemperatureTrace temp_trace;
	// Map of floorplan instance names to highlight
	private HashMap<String, MasterInst> highlights;
	
	// Current rendered time step
	private int time;
	// Current rendered layer
	private int layer;
	
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
		highlights = new HashMap<String, MasterInst>();

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
			updateImageSize();
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

		g.setPaintMode();
		// Fill background with some color
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_BACKGROUND));
		g.setPaint(COLOR_BACKGROUND);
		g.fillRect(0, 0, (int) getSize().getWidth(), (int) getSize().getHeight());
		
		// Get all the atomic instances
		List<MasterInst> atomics = Master.getFlatInstances(render_target);
		
		// If there is stuff to render
		if (render_target != null)
		{
			if (temp_trace != null)
			{
				paintTemperatures(g, atomics);
				paintOutlines(g, atomics, false);
			}
			else
				paintOutlines(g, atomics, true);
			paintHighlights(g);
		}
	}
	
	public synchronized void paintTemperatures(Graphics2D g, List<MasterInst> atomics)
	{
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
		g.setFont(FONT_TEMPTEXT);

		// Get maximum and minimum temperatures
		double max_temp = temp_trace.getMaxTemp();
		double min_temp = temp_trace.getMinTemp();
		// Get the temperatures for this time step
		TemperatureStep temp_step = temp_trace.getTemperatureSteps()[time];
	
		// Go through all atomic instances and paint them
		Iterator<MasterInst> it = atomics.iterator();
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();			
			// Get temperature for this instance
			int idx = temp_trace.getIdxMap().get(next_inst.n);
			
			// Get the temperature
			double temperature = temp_step.getTemperatures()[layer][idx];
			
			// Set the painting color based on the temperature
			Color temp_color = TemperatureColor.getColorRGB(temp_step.getTemperatures()[layer][idx], max_temp, min_temp);
			g.setPaint(temp_color);
			FloorplanRectangle rect = new FloorplanRectangle(next_inst, trans_x, trans_y, offset_x, offset_y, scale);			
			g.fillRect(rect.x, rect.y, rect.w, rect.h);
			
			// Write a string with the temperature information
            drawStringInBox(g, new Color(~temp_color.getRGB()),
            		String.format(FORMAT_TEMPTEXT, temperature), rect);			
		}
	}
	
	/**
	 * Draw all the outlines of blocks
	 */
	public synchronized void paintOutlines(Graphics2D g, List<MasterInst> atomics, boolean fill)
	{	
		// Go through all atomic instances and paint them
		Iterator<MasterInst> it = atomics.iterator();
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			// Find the bounding rectangle
			FloorplanRectangle rect = new FloorplanRectangle(next_inst, trans_x, trans_y, offset_x, offset_y, scale);			
			// Fill the rectangle if we are asked to
			if (fill)
			{
				if (next_inst.isFiller())
				{
					g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_FILLER));
					g.setPaint(COLOR_FILLER);		
				}
				else
				{
					g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_ATOMIC));
					g.setPaint(COLOR_ATOMIC);		
				}
				g.fillRect(rect.x, rect.y, rect.w, rect.h);
			}
			// Draw outline around rectangle
			g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_OUTLINE));
			g.setPaint(COLOR_OUTLINE);		
			g.drawRect(rect.x, rect.y, rect.w, rect.h);	
		}
		
		// Draw top-level border rectangle
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_BORDER));
		g.setPaint(COLOR_BORDER);
		FloorplanRectangle border = new FloorplanRectangle(Master.getBoundingBox(render_target), new Coord(0.0, 0.0),
				trans_x, trans_y, offset_x, offset_y, scale);
		g.drawRect(border.x,  border.y, border.w, border.h);
	}

	public synchronized void paintHighlights(Graphics2D g)
	{
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, ALPHA_HIGHLIGHTS));
		g.setPaint(COLOR_HIGHLIGHTS);

		// A list of atomics to highlight
		List<MasterInst> atomics = new LinkedList<MasterInst>();		
		// Iterate through all the highlights
		Iterator<MasterInst> it = highlights.values().iterator();		
		while(it.hasNext())
		{
			MasterInst next_inst = it.next();
			Master.addFlatInstances(atomics, next_inst.m, next_inst.x, next_inst.y, next_inst.n);
		}
		
		// Highlight and draw a halo around the highlighted object
		it = atomics.iterator();
		while(it.hasNext())
		{
			FloorplanRectangle rect = new FloorplanRectangle(it.next(), trans_x, trans_y, offset_x, offset_y, scale);
			Stroke old_stroke = g.getStroke();
			g.setStroke(new BasicStroke(LINEWIDTH_HIGHLIGHTS));
			g.drawRect(rect.x, rect.y, rect.w, rect.h);
			g.setStroke(old_stroke);
		}
	}
	
	private void drawStringInBox(Graphics2D g, Color c, String s, FloorplanRectangle rect)
	{
		// If there is enough room to write the temperature
		int string_w = (int) g.getFontMetrics().getStringBounds(s, g).getWidth();
		int string_h = (int) g.getFontMetrics().getStringBounds(s, g).getHeight();

		if ((string_w + 10 < rect.w) && (string_h + 10 < rect.h))
		{			
			int start_x = rect.w / 2 - string_w / 2;
			int start_y = rect.h / 2 - string_h / 2;
            g.setPaint(c);
    		g.scale(1, -1);
			g.drawString(s, rect.x + start_x, -(rect.y + start_y));
    		g.scale(1, -1);
		}
	}
	
	public HashMap<String, MasterInst> getHighlights()
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

	// Centers and fits the zoom
	public void zoom()
	{
		if (render_target != null)
		{
			Box b_box = Master.getBoundingBox(render_target);
			if(b_box.isValidBox())
			{
				// Set default scale	
				setScale(0.9 * Math.min(getSize().getWidth() / b_box.getWidth().toDouble(),
						getSize().getHeight() / b_box.getHeight().toDouble()));
				// Set translates
				trans_x = -b_box.llx.toDouble() - b_box.getWidth().toDouble() / 2;
				trans_y = -b_box.lly.toDouble() - b_box.getHeight().toDouble() / 2;
			}
		}
		else
		{
			trans_x = 0;
			trans_y = 0;
			setScale(MAX_SCALE);
		}
	}

	
	public void setTime(int time)
	{
		this.time = time;
	}
	
	public void setLayer(int layer)
	{
		this.layer = layer;
	}
	
	public void updateImageSize()
	{ 
		offset_x = getSize().width / 2;
		offset_y = getSize().height / 2;
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
	public Box getViewingArea()
	{
		Coord ll = pixelToCoord(0, getSize().height);
		Coord ur = pixelToCoord(getSize().width, 0);		
		return new Box(ll.x, ll.y, ur.x, ur.y);
	}
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
