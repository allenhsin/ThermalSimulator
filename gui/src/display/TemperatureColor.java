package display;

import java.awt.Color;

// Simple class with a helper that returns a color for a corresponding temperature
class TemperatureColor
{
	public static final double COLOR_POWER = 0.7;
	public static final double INV_COLOR_POWER = 1.0 / COLOR_POWER;
	
	static Color getColorRGB(double temp, double max_temp, double min_temp)
	{
		float delta = (float) Math.pow((temp - min_temp) / (max_temp - min_temp), COLOR_POWER);
		return new Color(1.0f * red(delta), 1.0f * green(delta), 1.0f * blue(delta));
	}
	
	static Color getColorGray(double temp, double max_temp, double min_temp)
	{
		float delta = (float) Math.pow((temp - min_temp) / (max_temp - min_temp), COLOR_POWER);
		return new Color(delta, delta, delta);
	}

	private static float red(float val)
	{
		if (val < 0.375f) return 0.0f;
		else if (val < 0.625f) return (val - 0.375f)/0.25f;
		else if (val < 0.875f) return 1.0f;
		else if (val < 1.0f) return (1.125f - val)/0.25f;
		else return 0.5f;
	}

	private static float green(float val)
	{
		if (val < 0.125f) return 0.0f;
		else if (val < 0.375f) return (val - 0.125f)/0.25f;
		else if (val < 0.625f) return 1.0f;
		else if (val < 0.875f) return (0.875f - val)/0.25f;
		else return 0.0f;
	}
	
	private static float blue(float val)
	{
		if (val < 0) return 0.5f;
		else if (val < 0.125f) return (0.5f + val/0.25f);
		else if (val < 0.375f) return 1.0f;
		else if (val < 0.625f) return (0.625f - val)/0.25f;
		else return 0.0f;
	}
	static double getTemp(double max_temp, double min_temp, Color color)
	{
		return Math.pow(color.getRed()/256f, INV_COLOR_POWER) * (max_temp - min_temp) + min_temp;
	}
	
}
