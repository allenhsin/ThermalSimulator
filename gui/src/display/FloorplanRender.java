package display;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import java.awt.AlphaComposite;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.image.*;
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
	// Mouse listener
	private RenderMouse mouse;
	// Keyboard listener
	private RenderKeyboard keyboard;		
	// Map of floorplan instances to highlight
	private List<FloorplanInst> highlights;
	
	// Current rendered time step
	private int time;
	
	// Scaling factor
	private double scale_x;
	private double scale_y;
	
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
		
		// Create mouse listener
		mouse = new RenderMouse(this);
		keyboard = new RenderKeyboard(this);
		
		// Highlights
		highlights = new LinkedList<FloorplanInst>();

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

		// Set default scale	
		scale_x = getPreferredSize().getWidth() / floorplan.getWidth();
		scale_y = getPreferredSize().getHeight() / floorplan.getHeight();
		
		// Set translates
		trans_x = -floorplan.getWidth() / 2;
		trans_y = -floorplan.getHeight() / 2;
		
	}
	
	/**
	 * Set the temperature trace to display
	 * @param temp_trace
	 */
	public void setTempTrace(TemperatureTrace temp_trace)
	{
		this.temp_trace = temp_trace;		
	}
	
	public synchronized void paintComponent (Graphics g0) 
	{
		super.paintComponent(g0);
		Graphics2D g = (Graphics2D) g0;

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
		// Get list of floorplan instances
		Vector<FloorplanInst> fp_insts = floorplan.getFloorplanInsts();
		
		// Get temperatures at this time step
		TemperatureStep temp_step = temp_trace.getTemperatureSteps()[time];

		// Go through every floorplan block
		for (int i = 0; i < fp_insts.size(); ++i)
		{
			FloorplanInst inst = fp_insts.get(i);

			// Get temperatures for the block
			int idx = temp_trace.getIdxMap().get(inst.f.getName());
			double temperature = temp_step.getTemperatures()[idx];
			
			g.setColor(TemperatureColor.getColor(temperature, temp_trace.getMaxTemp(), temp_trace.getMinTemp()));
			
			double x_raw = (inst.x + trans_x) * scale_x + offset_x;
			double y_raw = (inst.y + trans_y) * scale_y + offset_y;
			double w_raw = inst.f.getWidth() * scale_x;
			double h_raw = inst.f.getHeight() * scale_y;
			
			// Maybe a more efficient way to do this?
			int w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
			int h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
			
			g.fillRect(	(int) Math.round(x_raw),
						(int) Math.round(y_raw),
						(int) w,
						(int) h);
		}		
	}
	
	public synchronized void paintOutlines(Graphics2D g)
	{	
		// Get list of floorplan instances
		Vector<FloorplanInst> fp_insts = floorplan.getFloorplanInsts();
		
		// Draw block outlines in white
		g.setColor(Color.white);

		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 0.5));
		
		for (int i = 0; i < fp_insts.size(); ++i)
		{
			FloorplanInst inst = fp_insts.get(i);
			
			double x_raw = (inst.x + trans_x) * scale_x + offset_x;
			double y_raw = (inst.y + trans_y) * scale_y + offset_y;
			double w_raw = inst.f.getWidth() * scale_x;
			double h_raw = inst.f.getHeight() * scale_y;
			
			// Maybe a more efficient way to do this?
			int w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
			int h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
			
			g.drawRect(	(int) Math.round(x_raw),
						(int) Math.round(y_raw),
						(int) w,
						(int) h);
		}
	}

	public synchronized void paintHighlights(Graphics2D g)
	{
		// Highlight in white
		g.setColor(Color.white);
	
		// Set alpha blending
		g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float) 0.8));
		
		Iterator<FloorplanInst> it = highlights.iterator();
		
		while(it.hasNext())
		{
			FloorplanInst inst = it.next();
			
			double x_raw = (inst.x + trans_x) * scale_x + offset_x;
			double y_raw = (inst.y + trans_y) * scale_y + offset_y;
			double w_raw = inst.f.getWidth() * scale_x;
			double h_raw = inst.f.getHeight() * scale_y;
			
			// Maybe a more efficient way to do this?
			int w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
			int h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
			
			g.fillRect(	(int) Math.round(x_raw),
						(int) Math.round(y_raw),
						(int) w,
						(int) h);
		}	
	}

	public void repaintInst(FloorplanInst inst)
	{
		double x_raw = (inst.x + trans_x) * scale_x + offset_x;
		double y_raw = (inst.y + trans_y) * scale_y + offset_y;
		double w_raw = inst.f.getWidth() * scale_x;
		double h_raw = inst.f.getHeight() * scale_y;
		
		// Maybe a more efficient way to do this?
		int w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
		int h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
		
		repaint(	(int) Math.round(x_raw),
					(int) Math.round(y_raw),
					(int) w,
					(int) h);
	}
	
	public List<FloorplanInst> getHighlights()
	{
		return highlights;
	}
	
	// Moves the image
	public void translate(int x, int y)
	{
		trans_x += x / scale_x;
		trans_y += y / scale_y;
	}
	
	// Moves the image
	public void zoom(double scale_factor)
	{
		scale_x = scale_x * scale_factor;
		scale_y = scale_y * scale_factor;
	}
	
	public void setTime(int time)
	{
		this.time = time;
	}
	
}
