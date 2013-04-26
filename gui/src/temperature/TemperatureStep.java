package temperature;

import java.util.Scanner;

// Holds the temperature for a particular timestep
public class TemperatureStep 
{
	private TemperatureStep(int layers, int elements)
	{
		temperatures = new double[layers][elements];
		
		max_temp = Double.MIN_VALUE;
		min_temp = Double.MAX_VALUE;
	}
	
	public double[][] getTemperatures()
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

	public static TemperatureStep parseTemperatureStep(int layers, int elements, String line) throws Exception
	{
		// Create a new temperature step
		TemperatureStep temp = new TemperatureStep(layers, elements);
		// Use a scanner to parse the line
		Scanner s = new Scanner(line);
		int element = 0;
		int layer = 0;
		int total = 0;
		while(s.hasNext())
		{
			// Check
			if (total >= layers * elements) throw new Exception("Parsed line contains too many elements!");
			// Add the temperature for the element
			double temperature =  Double.parseDouble(s.next());
			temp.temperatures[layer][element++] = temperature;
			total++;
			// Update max and min temperatures
			if (temperature > temp.max_temp) temp.max_temp = temperature;
			else if (temperature < temp.min_temp) temp.min_temp = temperature;
			
			if (element == elements)
			{
				++layer;
				element = 0;
			}
		}
		
		// Check
		if (total != layers * elements) throw new Exception("Parsed line contains too few elements!");
				
		return temp;
	}
	
	// Maximum and minimum temperatures
	private double max_temp;
	private double min_temp;
	private double[][] temperatures;

}
