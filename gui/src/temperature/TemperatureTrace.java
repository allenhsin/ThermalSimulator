package temperature;

import java.util.HashMap;
import java.util.Scanner;
import java.io.*;

// A temperature trace class contains a list of temperatures at timesteps
public class TemperatureTrace 
{
	private TemperatureTrace(int timesteps)
	{
		idx_map = new HashMap<String, Integer>();
		temp_steps = new TemperatureStep[timesteps];

		max_temp = Double.MIN_VALUE;
		min_temp = Double.MAX_VALUE;
	}
	
	public HashMap<String, Integer> getIdxMap()
	{
		return idx_map;
	}
	
	public TemperatureStep[] getTemperatureSteps()
	{
		return temp_steps;
	}
	
	public double getMaxTemp()
	{
		return max_temp;
	}
	
	public double getMinTemp()
	{
		return min_temp;
	}

	public static TemperatureTrace	parseTemperatureTrace(File f) throws Exception
	{
		// Find number of lines in the given file
		FileReader fr = new FileReader(f);
		LineNumberReader lnr = new LineNumberReader(fr);
		lnr.skip(Long.MAX_VALUE);
		int lines = lnr.getLineNumber();
		fr.close();
		lnr.close();
		
		if (lines < 1) throw new Exception("Temperature trace file (" + 
				f.getCanonicalPath() + ") must contain at least 1 line!");

		// Create new temperature trace
		TemperatureTrace temp = new TemperatureTrace(lines-1);
		
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
			else if (step.getMinTemp() < temp.min_temp) temp.min_temp = step.getMinTemp();
		}
		
		// Check
		if (i != (lines - 1)) throw new Exception("Expected number of lines (" + lines +
				") did not match actual number of lines (" + i + ")!");
		
		return temp;
	}
	
	// Parses the number of Elements and returns the number of Elements
	private static int parseElements(TemperatureTrace temp, String line) throws Exception
	{
		// Use a scanner to parse the line
		Scanner s = new Scanner(line);
		int i = 0;
		
		while(s.hasNext())
			temp.idx_map.put(s.next(), i++);
		
		return i;
	}

	private HashMap<String, Integer> idx_map;
	private TemperatureStep[] temp_steps;

	// Maximum and minimum temperatures
	private double max_temp;
	private double min_temp;	
}
