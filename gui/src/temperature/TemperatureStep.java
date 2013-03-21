package temperature;

import java.util.Scanner;

// Holds the temperature for a particular timestep
public class TemperatureStep 
{
	
	private TemperatureStep(int elements)
	{
		temperatures = new double[elements];
		
		max_temp = Double.MIN_VALUE;
		min_temp = Double.MAX_VALUE;
	}
	
	public double[] getTemperatures()
	{
		return temperatures;
	}
	
	public double getMaxTemp()
	{
		return max_temp;
	}
	
	public double getMinTemp()
	{
		return min_temp;
	}

	public static TemperatureStep parseTemperatureStep(int elements, String line) throws Exception
	{
		// Create a new temperature step
		TemperatureStep temp = new TemperatureStep(elements);
		// Use a scanner to parse the line
		Scanner s = new Scanner(line);
		int i = 0;
		
		while(s.hasNext())
		{
			// Check
			if (i >= elements) throw new Exception("Parsed line contains too many elements!");
			// Add the temperature for the element
			double temperature =  Double.parseDouble(s.next());
			temp.temperatures[i++] = temperature;
			// Update max and min temperatures
			if (temperature > temp.max_temp) temp.max_temp = temperature;
			else if (temperature < temp.min_temp) temp.min_temp = temperature;
		}
		
		// Check
		if (i != elements) throw new Exception("Parsed line contains too few elements!");
				
		return temp;
	}
	
	// Maximum and minimum temperatures
	private double max_temp;
	private double min_temp;
	private double[] temperatures;

}
