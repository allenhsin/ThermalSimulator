package display;

import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.Point;

import java.awt.event.KeyListener;

public class RenderKeyboard implements KeyListener
{
	// The panel the listener is a part of
	RenderPanel panel;
	
	public RenderKeyboard(RenderPanel panel)
	{
		this.panel = panel;
		panel.getRender().addKeyListener(this);
	}

	public void keyPressed(KeyEvent e)
	{
		switch(e.getKeyCode())
		{
			// Left Arrow
			case 37: panel.getRender().translate(100, 0); 
				break;
			// Up Arrow
			case 38: panel.getRender().translate(0, -100); 
				break;
			// Right Arrow
			case 39: panel.getRender().translate(-100, 0); 
				break;
			// Down Arrow
			case 40: panel.getRender().translate(0, 100); 
				break;
			// Pg Up
			case 33: panel.getRender().zoom(1.2);
				break;
			// Pg Down
			case 34: panel.getRender().zoom(1 / 1.2);
				break;
			// Home
			case 36: panel.getRender().zoom();
		}
		panel.getRender().repaint();
	}

	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	public void keyTyped(KeyEvent e) {
//		System.out.println(e.getKeyChar());
		
	}
		
}
