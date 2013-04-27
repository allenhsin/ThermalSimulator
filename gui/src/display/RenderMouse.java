package display;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.AWTException;
import java.awt.Cursor;
import java.awt.Point;
import java.awt.Robot;

import javax.swing.event.MouseInputAdapter;

import temperature.TemperatureTrace;

public class RenderMouse extends MouseInputAdapter 
{	
	// The scaling step size
	public static final double SCALE_STEP = 1.1;
	
	// The panel the listener is a part of
	RenderPanel panel;
	
	public RenderMouse(RenderPanel panel)
	{
		this.panel = panel;
		panel.getRender().addMouseListener(this);
		panel.getRender().addMouseWheelListener(this);
		panel.getRender().addMouseMotionListener(this);
	}
	
	/**
	 * Turn the mouse into a crosshair when entering the render image
	 */
	public void mouseEntered(MouseEvent e)
	{
		panel.getRender().setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
	}

	/**
	 * Turn the mouse into the default mouse when exiting the render image
	 */
	public void mouseExited(MouseEvent e)
	{
		panel.getRender().setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	}
	
	public void mouseMoved(MouseEvent e)
	{
		Point p = e.getPoint();
		panel.setMouseCoord(p.x, p.y);
	}
	
	
	public void mouseClicked(MouseEvent e)
	{
		panel.getRender().requestFocus();
		Point p = e.getPoint();		
		if (e.getButton() == MouseEvent.BUTTON2)
		{
			panel.getRender().zoom();
		}
		else if (e.getButton() == MouseEvent.BUTTON3)
		{
			int x_shift = p.x - panel.getRender().getSize().width / 2;
			int y_shift = p.y - panel.getRender().getSize().height / 2;			
			panel.getRender().translate(-x_shift, y_shift);			
		}
		panel.getRender().repaint();
	}
	
	public void mouseWheelMoved(MouseWheelEvent e)
	{
		Point p = e.getPoint();

		int x_shift = p.x - panel.getRender().getSize().width / 2;
		int y_shift = p.y - panel.getRender().getSize().height / 2;
		double scale_factor = Math.pow(SCALE_STEP, -e.getWheelRotation());
		panel.getRender().zoom(scale_factor);
		panel.getRender().translate((int) (x_shift * (1 - scale_factor)), (int) -(y_shift * (1 - scale_factor)));
		panel.getRender().repaint();
	}
	
}
