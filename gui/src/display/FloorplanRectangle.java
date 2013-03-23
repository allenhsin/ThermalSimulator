package display;

import floorplan.Floorplan;

// Simple class that calculates render pixel coordinates from positions
class FloorplanRectangle 
{
	// Pixel coordinates
	int x, y, w, h;	
	// Constructor, calculates everything
	FloorplanRectangle(Floorplan f, double trans_x, double trans_y,
			double offset_x, double offset_y, double scale)
	{		
		double x_raw = (f.getX() + trans_x) * scale + offset_x;
		double y_raw = (f.getY() + trans_y) * scale + offset_y;
		double w_raw = f.getWidth() * scale;
		double h_raw = f.getHeight() * scale;
		
		// Maybe a more efficient way to do this?
		w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
		h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
		x = (int) Math.round(x_raw);
		y = (int) Math.round(y_raw);
	}
}
