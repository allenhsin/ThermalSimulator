package display;
import java.awt.*;

public interface Renderable 
{
	public abstract int getHeight();
	
	public abstract int getWidth();
	
	public abstract Color toColor(int x, int y);
}
