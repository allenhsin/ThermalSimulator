package main;

import java.io.*;
import java.awt.Dimension;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import gui.Floorplanner;

public class Main 
{
	public static void main (String[] args)
	{
		initialize(args);	
	}
	
    private static void setRuntimeOptions(OptionParser option_parser)
    {
        option_parser.addOption("-render_size", "RenderSize", true, "X_pixels,Y_pixels", true, "600,600",
                "Specify the size of the render window, in pixels");

        option_parser.addOption("-floorplan", "FloorplanFile", true, "File", true, "",
                "Specify a floorplan file to load on startup");

        option_parser.addOption("-temp_trace", "TempTrace", true, "File", true, "", 
                "Specify a temperature trace file to load on startup");
    }

	private static void initialize(String[] args)
	{
		OptionParser option_parser = new OptionParser();
		setRuntimeOptions(option_parser);		
		// Parse the options
		option_parser.parseArguments(args, System.err, System.out);
		
		try
		{
            // Set System L&F
	        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
	    } 
	    catch (Exception e)
	    {
	       // Not a big issue if we can't set the system-native look and feel
	    }
		

		File fplan_file = null;
		File temp_trace = null;
		
		// Load floorplan file on startup
		if(!option_parser.get("FloorplanFile").isEmpty())
			fplan_file = new File(option_parser.get("FloorplanFile"));
		
		// Load temperature trace on startup
		if(!option_parser.get("TempTrace").isEmpty())
			temp_trace = new File(option_parser.get("TempTrace"));
		
		// Parse render size
		Dimension render_size;
		try
		{
			String[] render_size_string = option_parser.get("RenderSize").split(",");
			render_size = new Dimension(Integer.parseInt(render_size_string[0]), Integer.parseInt(render_size_string[1]));
		}
		catch (Exception e)
		{
			render_size = new Dimension(600, 600);
		}
			
		SwingUtilities.invokeLater(new GUIRunnable(render_size, fplan_file, temp_trace));	
	}	
}

class GUIRunnable implements Runnable
{
	// Floorplanner GUI
	private Floorplanner gui;
	
	// Floorplan and temperature trace files loaded via command line args
	private File fplan_file, temp_trace;
	// Command-line set dimensions
	private Dimension dimension;
	
	GUIRunnable (Dimension dimension, File fplan_file, File temp_trace)
	{
		this.dimension = dimension;
		this.fplan_file = fplan_file;
		this.temp_trace = temp_trace;
	}
	
	public void run()
	{
		gui = new Floorplanner("Floorplanner", dimension);
		if (fplan_file != null)
			gui.openFloorplanFile(fplan_file);
		if (temp_trace != null)
			gui.openTemperatureTrace(temp_trace);
	}
}
