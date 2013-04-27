package display;

import floorplan.*;
import gui.EventsHelper;
import temperature.*;

import java.awt.Dimension;
import java.awt.Font;

import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSlider;

import java.awt.BorderLayout;
import java.util.*;

import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.JSpinner;
import javax.swing.SwingConstants;
import javax.swing.SpinnerNumberModel;

/**
 * A render panel holds the floorplan render and some other knobs associated
 * with viewing the floorplan render
 *
 */
public class RenderPanel extends JPanel
{

	// The floorplan render image
	private FloorplanRender render;
	// Temperature color scale
	private TemperatureScale scale;
	// Time slider panel
	private JPanel time_slider_panel;
	// The slider for controller the current time (for temperature renders)
	private JSlider time_slider;	
	// Render labels
	private JLabel render_name_text;
	
	// Mouse coordinates
	private JLabel mouse_coord_text;
	
	// Time slider labels
	private JLabel time_slider_file_text;
	private JLabel time_slider_time_text;
	// Remember the mapping between a master and the temperature trace
	private HashMap<Master, TemperatureTrace> temp_trace_map;
	private JPanel panel;
	private JSpinner layer_spinner;
	private JLabel lblLayer;
	private JPanel panel_1;
	
	public RenderPanel (Dimension image_size)
	{
		super ();		
		setBorder(new EmptyBorder(5, 5, 5, 5));
		setLayout(new BorderLayout());
		
		temp_trace_map = new HashMap<Master, TemperatureTrace>();
		
		render = new FloorplanRender (image_size);
		scale = new TemperatureScale();

		// Create mouse and keyboard listeners
		new RenderMouse(this);
		new RenderKeyboard(this);
		
		add(render, BorderLayout.CENTER);
		add(scale, BorderLayout.WEST);
		
		createTimeSliderPanel();

		scale.setVisible(false);
		time_slider.setEnabled(false);
		time_slider_panel.setVisible(false);

	}

	private void createTimeSliderPanel()
	{
		time_slider_panel = new JPanel();
		time_slider_panel.setLayout(new BorderLayout());

		time_slider_file_text = new JLabel("No Temperature Trace Loaded", JLabel.CENTER);
		time_slider_file_text.setFont(new Font("Courier New", Font.PLAIN, 12));
		time_slider_panel.add(time_slider_file_text, BorderLayout.SOUTH);
		add(time_slider_panel, BorderLayout.SOUTH);
		
		panel = new JPanel();
		time_slider_panel.add(panel, BorderLayout.EAST);
		panel.setLayout(new BorderLayout(0, 0));
		
		panel_1 = new JPanel();
		time_slider_panel.add(panel_1, BorderLayout.CENTER);
		panel_1.setLayout(new BorderLayout(0, 0));
		
		time_slider = new JSlider(0, 0, 0);
		panel_1.add(time_slider, BorderLayout.CENTER);
		time_slider.setEnabled(false);
		time_slider.setPaintTicks(true);
		time_slider.setPaintLabels(true);
		time_slider.setToolTipText("Time Tick");
		time_slider_time_text = new JLabel("Time tick: 0", SwingConstants.CENTER);
		panel_1.add(time_slider_time_text, BorderLayout.NORTH);
		time_slider.addChangeListener(new RenderTimeSliderListener(this));
		
		JPanel info_panel = new JPanel();
		info_panel.setLayout(new BorderLayout());
		
		render_name_text = new JLabel("No floorplan loaded");
		render_name_text.setFont(new Font("Courier New", Font.PLAIN, 12));
		mouse_coord_text = new JLabel("");
		mouse_coord_text.setFont(new Font("Courier New", Font.PLAIN, 12));
		info_panel.add(mouse_coord_text, BorderLayout.EAST);
		info_panel.add(render_name_text, BorderLayout.WEST);
		add(info_panel, BorderLayout.NORTH);
		
		lblLayer = new JLabel("Layer");
		panel.add(lblLayer, BorderLayout.NORTH);
		
		layer_spinner = new JSpinner();
		layer_spinner.setFont(new Font("Tahoma", Font.PLAIN, 12));
		layer_spinner.setModel(new SpinnerNumberModel(0, null, null, 1));
		layer_spinner.setEditor(new JSpinner.DefaultEditor(layer_spinner));
		layer_spinner.setPreferredSize(new Dimension(40, 10));
		panel.add(layer_spinner, BorderLayout.CENTER);
		layer_spinner.addChangeListener(new RenderLayerSpinnerListener(this));
	}

	/**
	 * Set the master instance to view
	 */
	public void setView(Master master)
	{
		// Tell the render to load the floorplan
		render.setRenderTarget(master);
		// If we are setting a non-null master
		if (master != null)
		{
			// Update render name text
			render_name_text.setText(master.getName());
			// Add to hash map a value for the temperature trace for this floorplan,
			// if it does not exist yet
			if (!temp_trace_map.containsKey(master))
				temp_trace_map.put(master, null);
			setTempTrace(temp_trace_map.get(master));
		}
		else render_name_text.setText("Not viewing any floorplans");
	}
	
	public void setTempTrace(TemperatureTrace trace)
	{
		if (trace == null)
		{
			render.setTempTrace(null);
			scale.setVisible(false);
			time_slider.setEnabled(false);
			time_slider.setVisible(false);
			time_slider_file_text.setText("No Temperature Trace Loaded");
			time_slider_panel.setVisible(false);
		}
		else if (trace.isCompatible(render.getRenderTarget()))
		{
			// Associate the currently rendered floorplan with this temperature trace
			temp_trace_map.put(render.getRenderTarget(), trace);
			// Tell the render to load the temperature trace
			render.setTempTrace(trace);	
			// Update and enable slider with time information
			int total_steps = trace.getTemperatureSteps().length;
			time_slider.setMaximum(total_steps - 1);
			time_slider.setValue(0);
			time_slider.setMinorTickSpacing(total_steps / 20);
			time_slider.setMajorTickSpacing(total_steps / 10);
			time_slider_file_text.setText(trace.getFile().getName());
			time_slider.setEnabled(true);
			time_slider.setVisible(true);
			scale.setVisible(true);			
			time_slider_panel.setVisible(true);
			layer_spinner.setModel(new SpinnerNumberModel(0, 0, trace.getNumLayers() - 1, 1));
			
			scale.setMaxTemp(trace.getMaxTemp());
			scale.setMinTemp(trace.getMinTemp());
		}
		else
		{
			Vector<String> report = trace.getCompatibilityReport(render.getRenderTarget());
			for (int i = 0; i < report.size(); ++i)
				System.out.println(report.get(i));
			JOptionPane.showMessageDialog(this, "Temperature trace not compatible" +
					" with currently viewed master.", "Failed to load", JOptionPane.WARNING_MESSAGE);
		}
	}
	
	public void setMouseCoord(int x, int y)
	{
		Coord coord = render.pixelToCoord(x, y);
		// Convert to um
		double x_um = coord.x.toDouble() * 1e6;
		double y_um = coord.y.toDouble() * 1e6;
		mouse_coord_text.setText(String.format("X: %+13.3f um    Y: %+13.3f um", x_um, y_um));
	}
	
	/**
	 * Unloads all temperature traces
	 */
	public void unloadTempTraces()
	{
		setTempTrace(null);
		temp_trace_map.clear();
	}
	
	public FloorplanRender getRender() { return render; }

	/**
	 * Update the time
	 */
	public void updateTime()
	{
		time_slider_time_text.setText("Time Tick: " + time_slider.getValue());
		render.setTime(time_slider.getValue());
		render.repaint();
	}
	
	/**
	 * Update the layer
	 */
	public void updateLayer()
	{
		render.setLayer(Integer.parseInt(layer_spinner.getValue().toString()));
		render.repaint();
	}
	
}

class RenderTimeSliderListener extends EventsHelper<RenderPanel>  implements ChangeListener
{
	RenderTimeSliderListener(RenderPanel owner)
	{
		super(owner);
	}
	
	/**
	 * When the time tick slider moves
	 */
	public void stateChanged(ChangeEvent e)
	{
		owner.updateTime();
	}	
}

class RenderLayerSpinnerListener extends EventsHelper<RenderPanel>  implements ChangeListener
{
	RenderLayerSpinnerListener(RenderPanel owner)
	{
		super(owner);
	}
	
	public void stateChanged(ChangeEvent e)
	{
		owner.updateLayer();
	}	
}
