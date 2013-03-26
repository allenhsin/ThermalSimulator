package display;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.Stroke;

import java.awt.AlphaComposite;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.image.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JComponent;
import javax.swing.border.BevelBorder;
import javax.swing.border.EmptyBorder;

import floorplan.*;
import temperature.*;

/** 
 * The floorplan render class is responsible for rendering the floorplan in various forms
 * as needed by the top-level tool
 */
public class FloorplanRender extends JComponent
{	
	// The floorplan to render
	private Floorplan floorplan;
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
	public void setFloorplan(Floorplan floorplan)
	{
		this.floorplan = floorplan;
		temp_trace = null;
		highlights.clear();		
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
		
		if (floorplan != null)
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
		// Paint outlines, starting with the top floorplan instance
		paintOutlines(g, floorplan);
		// Draw top-level border rectangle in cyan
		g.setColor(Color.cyan);
		FloorplanRectangle border = new FloorplanRectangle(floorplan, floorplan,
				trans_x, trans_y, offset_x, offset_y, scale);
		g.drawRect(border.x,  border.y, border.w, border.h);
	}
	
	/**
	 * Helper for painting a specific floorplan
	 */
	private synchronized void paintOutlines(Graphics2D g, Floorplan f)
	{
		// If it is a leaf, paint it
		if (f.isLeaf())
		{
			FloorplanRectangle rect = new FloorplanRectangle(f, floorplan,
					trans_x, trans_y, offset_x, offset_y, scale);			
			g.drawRect(rect.x, rect.y, rect.w, rect.h);				
		}
		// Otherwise recusively call and paint
		else
		{
			// Iterate through all sub-floorplans
			Iterator<Floorplan> it = f.getChildren().iterator();
			while(it.hasNext())
				paintOutlines(g, it.next());
		}
	}

	public synchronized void paintHighlights(Graphics2D g)
	{
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 0.5));
		g.setColor(Color.white);

		// Iterate through all highlights and select them
		Iterator<String> it = highlights.keySet().iterator();		
		while(it.hasNext())
			paintHighlights(g, floorplan.getChildrenMap().get(it.next()));
			
	}
	
	/**
	 * Helper for painting highlights
	 */
	private synchronized void paintHighlights(Graphics2D g, Floorplan f)
	{
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 0.5));
		// If it is a leaf, paint it
		if (f.isLeaf())
		{
			FloorplanRectangle rect = new FloorplanRectangle(f, floorplan,
					trans_x, trans_y, offset_x, offset_y, scale);			
			// Highlight in white
			g.drawRect(rect.x, rect.y, rect.w, rect.h);
			g.fillRect(rect.x, rect.y, rect.w, rect.h);		
		}
		// Otherwise recusively call and paint
		else
		{
			// Iterate through all sub-floorplans
			Iterator<Floorplan> it = f.getChildren().iterator();
			while(it.hasNext())
				paintHighlights(g, it.next());
		}
	}

	public void repaintInst(Floorplan f)
	{
		FloorplanRectangle rect = new FloorplanRectangle(f,
				floorplan, trans_x, trans_y, offset_x, offset_y, scale);					
		repaint(rect.x, rect.y, rect.w, rect.h);		
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
		// Set default scale	
		scale = 0.9 * Math.min(getPreferredSize().getWidth() / floorplan.getWidth(),
				getPreferredSize().getHeight() / floorplan.getHeight());
		
		// Set translates
		trans_x = -floorplan.getWidth() / 2;
		trans_y = -floorplan.getHeight() / 2;
	}
	
	public void setTime(int time)
	{
		this.time = time;
	}
	
	public Floorplan getFloorplan() { return floorplan; }
	public TemperatureTrace getTemperatureTrace() { return temp_trace; }
	
}
