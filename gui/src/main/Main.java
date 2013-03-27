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
		// Create a reference to the file
		File fplan_file;
		File temp_trace;
		int width;
		int height;

		if (args.length != 4)
		{
			fplan_file = null;
			temp_trace = null;
			height = 600;
			width = 600;
//			System.out.print("Usage: Main floorplan_file ttrace_file dimension_x dimension_y\n");
//			System.exit(1);
		}
		else
		{
			fplan_file = new File(args[0]);
			temp_trace = new File(args[1]);
			// Create screen dimensions
			width = Integer.parseInt(args[2]);
			height = Integer.parseInt(args[3]);
		}
		
		try
		{
            // Set System L&F
	        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
	    } 
	    catch (Exception e)
	    {
	       throw new Error(e);
	    }
		SwingUtilities.invokeLater(new ScreenRefresher(new Dimension(width, height), fplan_file, temp_trace));
//		new Thread(new ScreenRefresher(new Dimension(width, height), fplan)).start();
//		RenderWindow screen = new RenderWindow("Frame Buffer", pegasus.displayFrameBuffer());
//		RenderWindow nextScreen = new RenderWindow("Frame Buffer", pegasus.activeFrameBuffer());
		
	}
}

class ScreenRefresher implements Runnable
{
	// Floorplanner GUI
	private Floorplanner gui;
	
	// Floorplan and temperature trace files loaded via command line args
	private File fplan_file, temp_trace;
	// Command-line set dimensions
	private Dimension dimension;
	
	ScreenRefresher (Dimension dimension, File fplan_file, File temp_trace)
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
