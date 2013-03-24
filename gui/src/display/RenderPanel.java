package display;

import floorplan.*;
import temperature.*;

import java.awt.Dimension;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Color;

import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;

import java.awt.BorderLayout;
import java.util.*;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.MouseInputAdapter;

public class RenderPanel extends JPanel implements ChangeListener
{
	public RenderPanel (String title, Dimension image_size, Floorplan floorplan, TemperatureTrace temp_trace) 
	{
		super ();		
		image = new FloorplanRender (image_size);
		image.setFloorplan(floorplan);
		image.setTempTrace(temp_trace);
		
		add(image, BorderLayout.CENTER);
		createSlider(temp_trace);
	}

	public void redraw()
	{
		this.repaint();
	}
	
	private void createSlider(TemperatureTrace temp_trace)
	{
		int total_steps = temp_trace.getTemperatureSteps().length;
		slider = new JSlider(0, total_steps - 1, 0);
		slider.setMinorTickSpacing(total_steps / 20);
		slider.setMajorTickSpacing(total_steps / 10);
		slider.setPaintTicks(true);
		slider.setPaintLabels(true);
		slider.setToolTipText("Time Tick");
		slider.addChangeListener(this);
		add(slider, "South");
	}

	public void stateChanged(ChangeEvent e) {
		image.setTime(((JSlider) e.getSource()).getValue());
		image.repaint();
	}
	
	FloorplanRender image;
	JSlider slider;	
}

