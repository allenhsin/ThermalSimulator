package floorplan;

import javax.swing.table.AbstractTableModel;

public class InstanceTableModel extends AbstractTableModel
{
	// The master it points to, can be null for an empty table
	private Master master;
	
	public InstanceTableModel()
	{
		this.master = null;
	}
	
	public InstanceTableModel(Master master)
	{
		this.master = master;
	}
	
	/**	 * There are only 4 columns
	 */
	public int getColumnCount() { return 4; }

	/**
	 * Each sub-master instance is a row
	 */
	public int getRowCount() 
	{
		if (master == null)
			return 0;
		return master.getInstances().size();
	}

	/**
	 * Get the value of a cell in the table
	 */
	public Object getValueAt(int row, int col)
	{
		if (master != null)
		{
			switch(col)
			{
				case 0: return master.getInstances().get(row).n;
				case 1: return master.getInstances().get(row).m.getName();
				case 2: return master.getInstances().get(row).x;
				case 3: return master.getInstances().get(row).y;
			}
		}
		return null;
	}
	
	public String getColumnName(int col)
	{
		switch(col)
		{
			case 0: return "Instance";
			case 1: return "Master";
			case 2: return "X";
			case 3: return "Y";
		}
		return null;		
	}
	
	public Master getMaster()
	{
		return master;
	}
}
