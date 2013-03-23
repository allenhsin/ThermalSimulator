package display;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.Point;

import javax.swing.event.MouseInputAdapter;

public class RenderMouse extends MouseInputAdapter 
{
	public RenderMouse(FloorplanRender image)
	{
		this.image = image;
		image.addMouseListener(this);
		image.addMouseWheelListener(this);

	}
	
	public void mouseClicked(MouseEvent e)
	{
		Point p = e.getPoint();		
		if (e.getButton() == MouseEvent.BUTTON3)
		{
			int x_shift = p.x - image.getSize().width / 2;
			int y_shift = p.y - image.getSize().height / 2;
			
			image.translate(-x_shift, -y_shift);			
			image.repaint();
		}
	}
	
	public void mouseWheelMoved(MouseWheelEvent e)
	{
		Point p = e.getPoint();

		int x_shift = p.x - image.getSize().width / 2;
		int y_shift = p.y - image.getSize().height / 2;
		double scale_factor = Math.pow(SCALE_STEP, -e.getWheelRotation());
		image.zoom(scale_factor);
		image.translate((int) (x_shift * (1 - scale_factor)), (int) (y_shift * (1 - scale_factor)));
		image.repaint();
	}
	
	// The image that is being rendered
	FloorplanRender image;
	
	// The scaling step size
	public static final double SCALE_STEP = 1.1;
	
}
