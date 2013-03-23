package display;

import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.Point;

import java.awt.event.KeyListener;

public class RenderKeyboard implements KeyListener
{
	public RenderKeyboard(FloorplanRender image)
	{
		this.image = image;
		image.addKeyListener(this);
		image.setFocusable(true);
	}

	public void keyPressed(KeyEvent e)
	{
//		System.out.println(e.getKeyChar());
		
	}

	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	public void keyTyped(KeyEvent e) {
		System.out.println(e.getKeyChar());
		
	}
		
	// The image that is being rendered
	FloorplanRender image;
	
}
