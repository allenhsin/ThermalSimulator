package display;

import java.math.BigDecimal;
import java.math.MathContext;
import java.text.DecimalFormat;

import floorplan.Master;

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
//	FloorplanRectangle(Master target, Coord origin, double trans_x, double trans_y,
//			double offset_x, double offset_y, double scale)
//	{		
//		double x_raw = (origin.x + trans_x) * scale + offset_x;
//		double y_raw = (origin.y + trans_y) * scale + offset_y;
//		// TODO: Add a check that the target is a leaf master
//		double w_raw = target.getWidth() * scale;
//		double h_raw = target.getHeight() * scale;
//		
//		// Maybe a more efficient way to do this?
//		w = (int) (Math.round(x_raw + w_raw) - Math.round(x_raw));
//		h = (int) (Math.round(y_raw + h_raw) - Math.round(y_raw));
//		x = (int) Math.round(x_raw);
//		y = (int) Math.round(y_raw);
//	}
	
	FloorplanRectangle(Master target, Coord origin, double trans_x, double trans_y,
			int offset_x, int offset_y, double scale)
	{
		double llx = origin.x + trans_x;
		double lly = origin.y + trans_y;
		double urx = llx + target.getWidth();
		double ury = lly + target.getHeight();
		
		// Maybe a more efficient way to do this?
		w = (int) roundSigFigs(urx * scale, 6) - (int) roundSigFigs(llx * scale, 6);
		h = (int) roundSigFigs(ury * scale, 6) - (int) roundSigFigs(lly * scale, 6);
		x = (int) roundSigFigs(llx * scale, 6) + offset_x;
		y = (int) roundSigFigs(lly * scale, 6) + offset_y;
	}
	
	FloorplanRectangle(Box b, Coord origin, double trans_x, double trans_y,
			int offset_x, int offset_y, double scale)
	{
		if (b.isValidBox())
		{
			double llx = b.llx + origin.x + trans_x;
			double lly = b.lly + origin.y + trans_y;
			double urx = llx + b.getWidth();
			double ury = lly + b.getHeight();
			
			// Maybe a more efficient way to do this?
			w = (int) roundSigFigs(urx * scale, 6) - (int) roundSigFigs(llx * scale, 6);
			h = (int) roundSigFigs(ury * scale, 6) - (int) roundSigFigs(lly * scale, 6);
			x = (int) roundSigFigs(llx * scale, 6) + offset_x;
			y = (int) roundSigFigs(lly * scale, 6) + offset_y;
		}
		else
		{
			x = 0;
			y = 0;
			w = 0;
			h = 0;
		}
	}
	
	public static double roundSigFigs(double x, int sig_figs)
	{
		return BigDecimal.valueOf(x).round(new MathContext(sig_figs)).doubleValue();
	}
	
}
