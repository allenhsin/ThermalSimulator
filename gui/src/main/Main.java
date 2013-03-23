package main;

import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.SwingUtilities;
import display.*;
import floorplan.*;
import gui.Floorplanner;
import temperature.*;

public class Main 
{
	public static void main (String[] args)
	{
		if (args.length != 4)
		{
			System.out.println("Usage: Main floorplan_file ttrace_file dimension_x dimension_y");
			System.exit(1);
		}
		
		// Create a reference to the file
		File fplan_file = new File(args[0]);
		File temp_trace_file = new File(args[1]);
		// Create screen dimensions
		int width = Integer.parseInt(args[2]);
		int height = Integer.parseInt(args[3]);
		
		Floorplan fplan;
		TemperatureTrace temp_trace;
		
		try
		{			
			// Create the Floorplan
			fplan = Floorplan.parseFloorplan(fplan_file);
			temp_trace = TemperatureTrace.parseTemperatureTrace(temp_trace_file);
		}
		catch (Exception e)
		{
			throw new Error(e);
		}

		SwingUtilities.invokeLater(new ScreenRefresher(new Dimension(width, height), fplan, temp_trace));
//		new Thread(new ScreenRefresher(new Dimension(width, height), fplan)).start();
//		RenderWindow screen = new RenderWindow("Frame Buffer", pegasus.displayFrameBuffer());
//		RenderWindow nextScreen = new RenderWindow("Frame Buffer", pegasus.activeFrameBuffer());
		
	}
}

class ScreenRefresher implements Runnable
{
	private JFrame window;
	private Dimension dimension;
	private Floorplan fplan;
	private TemperatureTrace temp_trace;
	
	ScreenRefresher (Dimension dimension, Floorplan fplan, TemperatureTrace temp_trace)
	{
		this.dimension = dimension;
		this.fplan = fplan;
		this.temp_trace = temp_trace;
	}
	
	public void run()
	{
		this.window = new Floorplanner("Floorplanner", dimension, fplan);
//		this.window = new RenderWindow("Floorplan View", dimension, fplan, temp_trace);
	}
}
