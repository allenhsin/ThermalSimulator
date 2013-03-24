package display;

import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.Point;

import java.awt.event.KeyListener;

public class RenderKeyboard implements KeyListener
{
	// The image that is being rendered
	FloorplanRender image;
	
	public RenderKeyboard(FloorplanRender image)
	{
		this.image = image;
		image.addKeyListener(this);
	}

	public void keyPressed(KeyEvent e)
	{
		switch(e.getKeyCode())
		{
			// Left Arrow
			case 37: image.translate(100, 0); 
				break;
			// Up Arrow
			case 38: image.translate(0, -100); 
				break;
			// Right Arrow
			case 39: image.translate(-100, 0); 
				break;
			// Down Arrow
			case 40: image.translate(0, 100); 
				break;
			// Pg Up
			case 33: image.zoom(1.2);
				break;
			// Pg Down
			case 34: image.zoom(0.8);
				break;
			// Home
			case 36: image.zoom();
		}
		image.repaint();
	}

	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	public void keyTyped(KeyEvent e) {
//		System.out.println(e.getKeyChar());
		
	}
		
}
