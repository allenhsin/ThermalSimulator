package display;

import java.awt.Point;

import floorplan.MasterInst;

/** 
 * Simple class that calculates render pixel coordinates of a master instance
 * given an origin 
 */
class FloorplanRectangle 
{
	// Pixel coordinates
	int x, y, w, h;	
	// Constructor, calculates everything
	// The ref is used as the origin
	FloorplanRectangle(MasterInst target, Coord origin, double trans_x, double trans_y,
			double offset_x, double offset_y, double scale)
	{		
		double x_raw = (origin.x + trans_x) * scale + offset_x;
		double y_raw = (origin.y + trans_y) * scale + offset_y;
		// TODO: Add a check that the target is a leaf master
		double w_raw = target.m.getWidth() * scale;
		double h_raw = target.m.getHeight() * scale;
		
		// Maybe a more efficient way to do this?
		w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
		h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
		x = (int) Math.round(x_raw);
		y = (int) Math.round(y_raw);
	}
	
	FloorplanRectangle(Box b, Coord origin, double trans_x, double trans_y,
			double offset_x, double offset_y, double scale)
	{
		double x_raw = (b.llx + origin.x + trans_x) * scale + offset_x;
		double y_raw = (b.lly + origin.y + trans_y) * scale + offset_y;
		// TODO: Add a check that the target is a leaf master
		double w_raw = b.getWidth() * scale;
		double h_raw = b.getHeight() * scale;
		
		// Maybe a more efficient way to do this?
		w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
		h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
		x = (int) Math.round(x_raw);
		y = (int) Math.round(y_raw);
	}
	
}
