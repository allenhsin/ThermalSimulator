package display;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JComponent;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import javax.swing.border.EmptyBorder;

// Simple class with a helper that returns a color for a corresponding temperature
public class TemperatureScale extends JPanel
{
	public static final double COLOR_POWER = 0.7;
	public static final double INV_COLOR_POWER = 1.0 / COLOR_POWER;
	
	private TemperatureColorBar color_bar;
	
	public TemperatureScale() 
	{
		setBorder(new EmptyBorder(0, 0, 0, 5));
		setLayout(new BorderLayout(5, 5));
		
		color_bar = new TemperatureColorBar();
		add(color_bar, BorderLayout.CENTER);
		
//		JLabel lblColorScale = new JLabel("Color");
//		lblColorScale.setHorizontalAlignment(SwingConstants.CENTER);
//		add(lblColorScale, BorderLayout.NORTH);
	}
	
	public void setMaxTemp(double max_temp) { color_bar.setMaxTemp(max_temp); }
	public void setMinTemp(double min_temp) { color_bar.setMinTemp(min_temp); }

	static float getScale(double temp, double max_temp, double min_temp)
	{
		return (float) Math.pow((temp - min_temp) / (max_temp - min_temp), COLOR_POWER);
	}
	
	static Color getColorRGB(double temp, double max_temp, double min_temp)
	{
		float delta = getScale(temp, max_temp, min_temp);
		return new Color(red(delta), green(delta), blue(delta));
	}
	
	static Color getColorGray(double temp, double max_temp, double min_temp)
	{
		float delta = getScale(temp, max_temp, min_temp);
		return new Color(delta, delta, delta);
	}

	static float red(float val)
	{
		if (val < 0.375f) return 0.0f;
		else if (val < 0.625f) return (val - 0.375f)/0.25f;
		else if (val < 0.875f) return 1.0f;
		else if (val < 1.0f) return (1.125f - val)/0.25f;
		else return 0.5f;
	}

	static float green(float val)
	{
		if (val < 0.125f) return 0.0f;
		else if (val < 0.375f) return (val - 0.125f)/0.25f;
		else if (val < 0.625f) return 1.0f;
		else if (val < 0.875f) return (0.875f - val)/0.25f;
		else return 0.0f;
	}
	
	static float blue(float val)
	{
		if (val < 0) return 0.5f;
		else if (val < 0.125f) return (0.5f + val/0.25f);
		else if (val < 0.375f) return 1.0f;
		else if (val < 0.625f) return (0.625f - val)/0.25f;
		else return 0.0f;
	}
}

class TemperatureColorBar extends JComponent
{
	public static final Font FONT_TEMPTEXT = new Font("Arial", Font.PLAIN, 8);
	public static final String FORMAT_TEMPTEXT = "%5.2f";
	
	private double max_temp;
	private double min_temp;
	
	public TemperatureColorBar()
	{
		max_temp = 400;
		min_temp = 0;

		setPreferredSize(new Dimension(35, 0));		
	}
	
	public synchronized void paintComponent (Graphics g0) 
	{
		super.paintComponent(g0);
		Graphics2D g = (Graphics2D) g0;
		g.translate(0, getHeight());		
		g.setPaintMode();
		
		int num_steps = Math.max(Math.min(getHeight() / 20, 10), 1);
		int step = (int) Math.ceil((double) getHeight() / (double) num_steps);
		// Paint the color scale
		for (int i = 0; i < getHeight(); i++)
		{
			double temp = ((float) i / (float) getHeight()) * (max_temp - min_temp) + min_temp;
			Color color = TemperatureScale.getColorRGB(temp, max_temp, min_temp);
			g.setPaint(color);
			g.drawRect(0, -i, getSize().width, 1);
		}
		
		for (int i = 0; i < getHeight(); i++)
		{
			double temp = ((float) i / (float) getHeight()) * (max_temp - min_temp) + min_temp;
			if (i % step == 0)
			{
				Color color = new Color(~TemperatureScale.getColorRGB(temp, max_temp, min_temp).getRGB());
				g.setPaint(color);
				g.drawRect(0, -i, getSize().width, 1);
				g.drawString(String.format(FORMAT_TEMPTEXT, temp), 1, -i - 1);
			}
		}
		
	}
	
	public double getMaxTemp() { return max_temp; }
	public double getMinTemp() { return min_temp; }
	public void setMaxTemp(double max_temp) { this.max_temp = max_temp; }
	public void setMinTemp(double min_temp) { this.min_temp = min_temp; }
	
}
