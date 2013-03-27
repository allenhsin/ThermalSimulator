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

import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.MouseInputAdapter;

/**
 * A render panel holds the floorplan render and some other knobs associated
 * with viewing the floorplan render
 *
 */
public class RenderPanel extends JPanel implements ChangeListener
{

	// The floorplan render image
	private FloorplanRender render;
	// The slider for controller the current time (for temperature renders)
	private JSlider time_slider;	

	// Render labels
	private JLabel render_name_text;
	
	// Time slider labels
	private JLabel time_slider_file_text;
	private JLabel time_slider_time_text;
	// Remember the mapping between a master and the temperature trace
	private HashMap<Master, TemperatureTrace> temp_trace_map;
	
	// Mouse listener
	private RenderMouse mouse;
	// Keyboard listener
	private RenderKeyboard keyboard;

	public RenderPanel (Dimension image_size) 
	{
		super ();		
		setBorder(new EmptyBorder(5, 5, 5, 5));
		setLayout(new BorderLayout());
		
		temp_trace_map = new HashMap<Master, TemperatureTrace>();
		
		render = new FloorplanRender (image_size);
		mouse = new RenderMouse(this);
		keyboard = new RenderKeyboard(this);
		
		add(render, BorderLayout.CENTER);
		createTimeSliderPanel();
	}

	private void createTimeSliderPanel()
	{
		JPanel time_slider_panel = new JPanel();
		time_slider_panel.setLayout(new BorderLayout());
		
		time_slider = new JSlider(0, 0, 0);
		time_slider.setEnabled(false);
		time_slider.setPaintTicks(true);
		time_slider.setPaintLabels(true);
		time_slider.setToolTipText("Time Tick");
		time_slider.addChangeListener(this);

		render_name_text = new JLabel("No floorplan loaded");
		add(render_name_text, BorderLayout.NORTH);
		
		time_slider_file_text = new JLabel("No Temperature Trace Loaded", JLabel.CENTER);
		time_slider_time_text = new JLabel("Time tick: 0", JLabel.CENTER);
		time_slider_panel.add(time_slider, BorderLayout.CENTER);
		time_slider_panel.add(time_slider_time_text,  BorderLayout.NORTH);
		time_slider_panel.add(time_slider_file_text, BorderLayout.SOUTH);
		add(time_slider_panel, BorderLayout.SOUTH);
		
	}

	/**
	 * Set the master instance to view
	 */
	public void setView(MasterInst master_inst)
	{
		// Tell the render to load the floorplan
		render.setRenderTarget(master_inst);

		// Update render name text
		render_name_text.setText(master_inst.m.getName());
		
		// Add to hash map a value for the temperature trace for this floorplan,
		// if it does not exist yet
		if (!temp_trace_map.containsKey(master_inst.m))
			temp_trace_map.put(master_inst.m, null);
		setTempTrace(temp_trace_map.get(master_inst.m));
		
	}
	
	public void setTempTrace(TemperatureTrace trace)
	{
//		// Associate the currently rendered floorplan with this temperature trace
//		temp_trace_map.put(render.getFloorplan().getMaster(), trace);
//		// Tell the render to load the temperature trace
//		render.setTempTrace(trace);	
//		if (trace != null)
//		{
//			// Update and enable slider with time information
//			int total_steps = trace.getTemperatureSteps().length;
//			time_slider.setMaximum(total_steps - 1);
//			time_slider.setValue(0);
//			time_slider.setMinorTickSpacing(total_steps / 20);
//			time_slider.setMajorTickSpacing(total_steps / 10);
//			time_slider_file_text.setText(trace.getFile().getName());
//			time_slider.setEnabled(true);
//		}
//		else
//		{
//			time_slider.setEnabled(false);
//			time_slider_file_text.setText("No Temperature Trace Loaded");
//		}
	}
	
	/**
	 * Unloads all temperature traces
	 */
	public void unloadTempTrace()
	{
		render.setTempTrace(null);
		temp_trace_map.clear();
	}
	
	public FloorplanRender getRender() { return render; }

	/**
	 * When the time tick slider moves
	 */
	public void stateChanged(ChangeEvent e)
	{
		JSlider time_slider = (JSlider) e.getSource();
		time_slider_time_text.setText("Time Tick: " + time_slider.getValue());
		render.setTime(time_slider.getValue());
		render.repaint();
	}	
}

