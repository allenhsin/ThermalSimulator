package temperature;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Vector;
import java.io.*;

import floorplan.Master;
import floorplan.MasterInst;

// A temperature trace class contains a list of temperatures at timesteps
public class TemperatureTrace 
{	
	// File source
	private File file;
	
	// Temperature information
	private Hashtable<String, Integer> idx_map;
	private TemperatureStep[] temp_steps;

	// Maximum and minimum temperatures
	private double max_temp;
	private double min_temp;	

	private TemperatureTrace(int timesteps)
	{
		idx_map = new Hashtable<String, Integer>();
		temp_steps = new TemperatureStep[timesteps];

		max_temp = Double.MIN_VALUE;
		min_temp = Double.MAX_VALUE;		
	}
	
	/**
	 *  Checks whether the Temperature trace is compatible with a master
	 */
	public boolean isCompatible(Master m)
	{
		return (getCompatibilityReport(m).size() == 0);
	}
	
	/**
	 *  Gets a compatibility report of the current temperature file with a master
	 */
	public Vector<String> getCompatibilityReport(Master m)
	{
		// Create report to return
		Vector<String> report = new Vector<String>();
		// Get all the atomics of the master
		Hashtable<String, MasterInst> atomics = m.getAllAtomics();
		// Check temperature trace index map to see if those atomics exist
		Iterator<String> trace_key_it = idx_map.keySet().iterator();
		while(trace_key_it.hasNext())
		{
			String trace_key = trace_key_it.next();
			if (!atomics.containsKey(trace_key))
				report.add("Trace key '" + trace_key + "' does not map to an atomic");
		}		
		// Check atomics of the master to see if trace keys exist
		Iterator<String> atomics_it = atomics.keySet().iterator();
		while(atomics_it.hasNext())
		{
			String atomic_name = atomics_it.next();
			if (!idx_map.containsKey(atomic_name))
				report.add("Atomic '" + atomic_name + "' does not map to an entry in the temperature trace");
		}		
		return report;
	}
	
	public Hashtable<String, Integer> getIdxMap() { return idx_map; }	
	public TemperatureStep[] getTemperatureSteps() { return temp_steps; }	
	public double getMaxTemp() { return max_temp; }	
	public double getMinTemp() { return min_temp; }	
	public File getFile() { return file; }

	public static TemperatureTrace	parseTemperatureTrace(File f) throws Exception
	{
		// Find number of lines in the given file
		int lines = getNumberLines(f);
		// Checks the number of lines
		if (lines < 1) throw new Exception("Temperature trace file (" + 
				f.getCanonicalPath() + ") must contain at least 1 line!");

		// Create new temperature trace
		TemperatureTrace temp = new TemperatureTrace(lines-1);
		temp.file = f;
		
		// Create scanner
		Scanner s = new Scanner(f);
		
		// Read the first line to get all the Elements
		int elements = parseElements(temp, s.nextLine());
		
		// Parse each line of the temperature trace
		int i = 0;
		while (s.hasNextLine())
		{
			TemperatureStep step = TemperatureStep.parseTemperatureStep(elements, s.nextLine());
			temp.temp_steps[i++] = step;
			if (step.getMaxTemp() > temp.max_temp) temp.max_temp = step.getMaxTemp();
			if (step.getMinTemp() < temp.min_temp) temp.min_temp = step.getMinTemp();
		}
		
		// Check
		if (i != (lines - 1)) throw new Exception("Expected number of lines (" + lines +
				") did not match actual number of lines (" + i + ")!");
		
		return temp;
	}
	
	/**
	 * Returns the number of lines in a file
	 */
	private static int getNumberLines(File f) throws Exception
	{
		FileReader fr = new FileReader(f);
		LineNumberReader lnr = new LineNumberReader(fr);
		lnr.skip(Long.MAX_VALUE);
		int lines = lnr.getLineNumber();
		fr.close();
		lnr.close();
		
		return lines;
	}
	
	/**
	 *  Parses the number of Elements and returns the number of Elements
	 */
	private static int parseElements(TemperatureTrace temp, String line)
	{
		// Use a scanner to parse the line
		Scanner s = new Scanner(line);
		int i = 0;
		
		while(s.hasNext())
			temp.idx_map.put(s.next(), i++);
		
		return i;
	}

}
