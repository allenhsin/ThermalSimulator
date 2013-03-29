package gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.math.BigDecimal;
import java.util.Vector;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;

public class InstanceDialogBox extends JDialog
{
	// Buttons
	private JButton button_ok;
	private JButton button_cancel;
	
	// Text boxes
	private JTextField text_instance_name;
	private JTextField text_x_position;
	private JTextField text_y_position;
	private JTextField text_width;
	private JTextField text_height;
	// Check box
	private JCheckBox check_atomic;
	// Instance master combo box
	private JComboBox combo_instance_master;

	// Array parameters
	private JCheckBox check_array;
	private JTextField text_num_x;
	private JTextField text_num_y;
	private JTextField text_dx;
	private JTextField text_dy;
	
	// Masters
	private MasterMap masters;
	
	// New instances to return
	private Vector<MasterInst> new_insts;
		
	private InstanceDialogBox(JFrame frame, JComponent location_comp, MasterMap masters) 
	{
		super(frame, "New Instance", true);

		this.masters = masters;

		layoutGUI();
		addListeners();
		
		setEditAtomic(true);
		setEditArray(false);
		
		pack();
		setLocationRelativeTo(location_comp);
		setVisible(true);
	}
	
	/**
	 * Layouts the GUI and instantiates elements
	 */
	private void layoutGUI()
	{
		getContentPane().setLayout(new BorderLayout(0, 0));
		
		JPanel panel_1 = new JPanel();
		getContentPane().add(panel_1, BorderLayout.NORTH);
		panel_1.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		button_ok = new JButton("Ok");
//		button_ok.addActionListener(this);
		panel_1.add(button_ok);
		
		button_cancel = new JButton("Cancel");
//		button_cancel.addActionListener(this);
		panel_1.add(button_cancel);
		
		JPanel panel_2 = new JPanel();
		getContentPane().add(panel_2, BorderLayout.CENTER);
		GridBagLayout gbl_panel_2 = new GridBagLayout();
		gbl_panel_2.columnWidths = new int[]{10, 0, 100, 50, 0, 0, 10};
		gbl_panel_2.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		gbl_panel_2.columnWeights = new double[]{0.0, 0.0, 1.0, 1.0, 0.0, 0.0, Double.MIN_VALUE};
		gbl_panel_2.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		panel_2.setLayout(gbl_panel_2);
		
		JLabel label_instance_name = new JLabel("Instance Name");
		GridBagConstraints gbc_label_instance_name = new GridBagConstraints();
		gbc_label_instance_name.anchor = GridBagConstraints.EAST;
		gbc_label_instance_name.insets = new Insets(0, 0, 5, 5);
		gbc_label_instance_name.gridx = 1;
		gbc_label_instance_name.gridy = 1;
		panel_2.add(label_instance_name, gbc_label_instance_name);
		
		text_instance_name = new JTextField();
		GridBagConstraints gbc_text_instance_name = new GridBagConstraints();
		gbc_text_instance_name.gridwidth = 3;
		gbc_text_instance_name.insets = new Insets(0, 0, 5, 5);
		gbc_text_instance_name.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_instance_name.gridx = 2;
		gbc_text_instance_name.gridy = 1;
		panel_2.add(text_instance_name, gbc_text_instance_name);
		text_instance_name.setColumns(10);
		
		JLabel label_instance_master = new JLabel("Instance Master");
		GridBagConstraints gbc_label_instance_master = new GridBagConstraints();
		gbc_label_instance_master.anchor = GridBagConstraints.EAST;
		gbc_label_instance_master.insets = new Insets(0, 0, 5, 5);
		gbc_label_instance_master.gridx = 1;
		gbc_label_instance_master.gridy = 2;
		panel_2.add(label_instance_master, gbc_label_instance_master);
		
		combo_instance_master = new JComboBox();
		GridBagConstraints gbc_combo_instance_master = new GridBagConstraints();
		gbc_combo_instance_master.gridwidth = 2;
		gbc_combo_instance_master.insets = new Insets(0, 0, 5, 5);
		gbc_combo_instance_master.fill = GridBagConstraints.HORIZONTAL;
		gbc_combo_instance_master.gridx = 2;
		gbc_combo_instance_master.gridy = 2;
		combo_instance_master.setModel(new DefaultComboBoxModel(masters.toArray()));
		panel_2.add(combo_instance_master, gbc_combo_instance_master);
		
		check_atomic = new JCheckBox("Atomic");
		check_atomic.setSelected(true);
		GridBagConstraints gbc_check_atomic = new GridBagConstraints();
		gbc_check_atomic.insets = new Insets(0, 0, 5, 5);
		gbc_check_atomic.gridx = 4;
		gbc_check_atomic.gridy = 2;
		panel_2.add(check_atomic, gbc_check_atomic);
		
		JLabel label_x_position = new JLabel("X Position");
		GridBagConstraints gbc_label_x_position = new GridBagConstraints();
		gbc_label_x_position.anchor = GridBagConstraints.EAST;
		gbc_label_x_position.insets = new Insets(0, 0, 5, 5);
		gbc_label_x_position.gridx = 1;
		gbc_label_x_position.gridy = 4;
		panel_2.add(label_x_position, gbc_label_x_position);
		
		text_x_position = new JTextField();
		GridBagConstraints gbc_text_x_position = new GridBagConstraints();
		gbc_text_x_position.insets = new Insets(0, 0, 5, 5);
		gbc_text_x_position.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_x_position.gridx = 2;
		gbc_text_x_position.gridy = 4;
		panel_2.add(text_x_position, gbc_text_x_position);
		text_x_position.setColumns(10);
		
		JLabel label_y_position = new JLabel("Y Position");
		GridBagConstraints gbc_label_y_position = new GridBagConstraints();
		gbc_label_y_position.anchor = GridBagConstraints.EAST;
		gbc_label_y_position.insets = new Insets(0, 0, 5, 5);
		gbc_label_y_position.gridx = 1;
		gbc_label_y_position.gridy = 5;
		panel_2.add(label_y_position, gbc_label_y_position);
		
		text_y_position = new JTextField();
		GridBagConstraints gbc_text_y_position = new GridBagConstraints();
		gbc_text_y_position.insets = new Insets(0, 0, 5, 5);
		gbc_text_y_position.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_y_position.gridx = 2;
		gbc_text_y_position.gridy = 5;
		panel_2.add(text_y_position, gbc_text_y_position);
		text_y_position.setColumns(10);
		
		JLabel label_width = new JLabel("Width");
		GridBagConstraints gbc_label_width = new GridBagConstraints();
		gbc_label_width.anchor = GridBagConstraints.EAST;
		gbc_label_width.insets = new Insets(0, 0, 5, 5);
		gbc_label_width.gridx = 1;
		gbc_label_width.gridy = 6;
		panel_2.add(label_width, gbc_label_width);
		
		text_width = new JTextField();
		GridBagConstraints gbc_text_width = new GridBagConstraints();
		gbc_text_width.insets = new Insets(0, 0, 5, 5);
		gbc_text_width.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_width.gridx = 2;
		gbc_text_width.gridy = 6;
		panel_2.add(text_width, gbc_text_width);
		text_width.setColumns(10);
		
		JLabel label_height = new JLabel("Height");
		GridBagConstraints gbc_label_height = new GridBagConstraints();
		gbc_label_height.anchor = GridBagConstraints.EAST;
		gbc_label_height.insets = new Insets(0, 0, 5, 5);
		gbc_label_height.gridx = 1;
		gbc_label_height.gridy = 7;
		panel_2.add(label_height, gbc_label_height);
		
		text_height = new JTextField();
		GridBagConstraints gbc_text_height = new GridBagConstraints();
		gbc_text_height.insets = new Insets(0, 0, 5, 5);
		gbc_text_height.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_height.gridx = 2;
		gbc_text_height.gridy = 7;
		panel_2.add(text_height, gbc_text_height);
		text_height.setColumns(10);
		
		check_array = new JCheckBox("Array");
		GridBagConstraints gbc_chckbxArray = new GridBagConstraints();
		gbc_chckbxArray.insets = new Insets(0, 0, 5, 5);
		gbc_chckbxArray.gridx = 2;
		gbc_chckbxArray.gridy = 8;
		panel_2.add(check_array, gbc_chckbxArray);
		
		JLabel lblNumX = new JLabel("Num X");
		GridBagConstraints gbc_lblNumX = new GridBagConstraints();
		gbc_lblNumX.insets = new Insets(0, 0, 5, 5);
		gbc_lblNumX.anchor = GridBagConstraints.EAST;
		gbc_lblNumX.gridx = 1;
		gbc_lblNumX.gridy = 9;
		panel_2.add(lblNumX, gbc_lblNumX);
		
		text_num_x = new JTextField();
		GridBagConstraints gbc_text_num_x = new GridBagConstraints();
		gbc_text_num_x.insets = new Insets(0, 0, 5, 5);
		gbc_text_num_x.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_num_x.gridx = 2;
		gbc_text_num_x.gridy = 9;
		panel_2.add(text_num_x, gbc_text_num_x);
		text_num_x.setColumns(10);
		
		JLabel lblNumY = new JLabel("Num Y");
		GridBagConstraints gbc_lblNumY = new GridBagConstraints();
		gbc_lblNumY.insets = new Insets(0, 0, 5, 5);
		gbc_lblNumY.anchor = GridBagConstraints.EAST;
		gbc_lblNumY.gridx = 1;
		gbc_lblNumY.gridy = 10;
		panel_2.add(lblNumY, gbc_lblNumY);
		
		text_num_y = new JTextField();
		GridBagConstraints gbc_text_num_y = new GridBagConstraints();
		gbc_text_num_y.insets = new Insets(0, 0, 5, 5);
		gbc_text_num_y.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_num_y.gridx = 2;
		gbc_text_num_y.gridy = 10;
		panel_2.add(text_num_y, gbc_text_num_y);
		text_num_y.setColumns(10);
		
		JLabel lblDistanceX = new JLabel("Distance X");
		GridBagConstraints gbc_lblDistanceX = new GridBagConstraints();
		gbc_lblDistanceX.insets = new Insets(0, 0, 5, 5);
		gbc_lblDistanceX.anchor = GridBagConstraints.EAST;
		gbc_lblDistanceX.gridx = 1;
		gbc_lblDistanceX.gridy = 11;
		panel_2.add(lblDistanceX, gbc_lblDistanceX);
		
		text_dx = new JTextField();
		GridBagConstraints gbc_text_dx = new GridBagConstraints();
		gbc_text_dx.insets = new Insets(0, 0, 5, 5);
		gbc_text_dx.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_dx.gridx = 2;
		gbc_text_dx.gridy = 11;
		panel_2.add(text_dx, gbc_text_dx);
		text_dx.setColumns(10);
		
		JLabel lblDistanceY = new JLabel("Distance Y");
		GridBagConstraints gbc_lblDistanceY = new GridBagConstraints();
		gbc_lblDistanceY.insets = new Insets(0, 0, 5, 5);
		gbc_lblDistanceY.anchor = GridBagConstraints.EAST;
		gbc_lblDistanceY.gridx = 1;
		gbc_lblDistanceY.gridy = 12;
		panel_2.add(lblDistanceY, gbc_lblDistanceY);
		
		text_dy = new JTextField();
		GridBagConstraints gbc_text_dy = new GridBagConstraints();
		gbc_text_dy.insets = new Insets(0, 0, 5, 5);
		gbc_text_dy.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_dy.gridx = 2;
		gbc_text_dy.gridy = 12;
		panel_2.add(text_dy, gbc_text_dy);
		text_dy.setColumns(10);
	}
	
	/**
	 * Adds listeners to various GUI elements
	 */
	private void addListeners()
	{
		check_atomic.addItemListener(new InstanceAtomicListener(this));
		check_array.addItemListener(new InstanceArrayListener(this));
		button_ok.addActionListener(new InstanceDialogButtonListener(this));
		button_cancel.addActionListener(new InstanceDialogButtonListener(this));
	}

	/**
	 * Sets whether we are creating an element array
	 */
	public void setEditArray(boolean array)
	{
		if (array)
		{
			text_num_x.setEnabled(true);
			text_num_y.setEnabled(true);
			text_dx.setEnabled(true);
			text_dy.setEnabled(true);
		}
		else
		{
			text_num_x.setEnabled(false);
			text_num_y.setEnabled(false);
			text_dx.setEnabled(false);
			text_dy.setEnabled(false);
		}
	}

	/**
	 * Sets whether we are creating atomic instance(s)
	 */
	public void setEditAtomic(boolean atomic)
	{	
		if (atomic)
		{
			text_height.setEnabled(true);
			text_width.setEnabled(true);
			combo_instance_master.setEnabled(false);
		}
		else
		{	
			text_height.setEnabled(false);
			text_width.setEnabled(false);				
			combo_instance_master.setEnabled(true);
			if (combo_instance_master.getItemCount() > 0)
				combo_instance_master.setSelectedIndex(0);
		}
	}

	/**
	 * Creates the instances and adds them to the list of new instances
	 * returns if the operation is successful
	 */
	public boolean createInstances()
	{
		// If the input check did not succeed, report failure
		if (!checkInputs())
			return false;
		
		new_insts = new Vector<MasterInst>();
		if(check_array.isSelected())
		{
			int num_x = Integer.parseInt(text_num_x.getText());
			int num_y = Integer.parseInt(text_num_y.getText());
			
			BigDecimal d_x = BigDecimal.valueOf(Double.parseDouble(text_dx.getText()));
			BigDecimal d_y = BigDecimal.valueOf(Double.parseDouble(text_dy.getText()));

			// Will number horizontally first, then vertically
			for (int i = 0; i < num_y; ++i)
			{
				for (int j = 0; j < num_x; ++j)
				{
					createInstance("_" + (i * num_x + j), 
						d_x.multiply(BigDecimal.valueOf(j)).doubleValue(), 
						d_y.multiply(BigDecimal.valueOf(i)).doubleValue());
				}
			}
		}
		else createInstance("", 0.0, 0.0);
		return true;
	}
	
	/**
	 * Creates one instance, based on the numbers in the text boxes
	 */
	private void createInstance(String name_postpend, double x_offset, double y_offset)
	{
		Master new_inst_master = null;
		// First check if it is atomic or not
		if (check_atomic.isSelected())
		{
			// Parse height/width
			double width = Double.parseDouble(text_width.getText());
			double height = Double.parseDouble(text_height.getText());
			new_inst_master = new Master(width, height);
		}
		else
		{
			if (combo_instance_master.getSelectedIndex() != -1)
				new_inst_master = (Master) combo_instance_master.getSelectedItem();
		}
		// Get instance name
		String inst_name = text_instance_name.getText() + name_postpend;
		// Get X/Y coordinates
		double x = Double.parseDouble(text_x_position.getText()) + x_offset;
		double y = Double.parseDouble(text_y_position.getText()) + y_offset;
		
		// Create the new instance
		new_insts.add(new MasterInst(new_inst_master, inst_name, x, y));
	}
	
	/**
	 * Show the dialog and returns a vector of the new instances to add
	 */
	public static Vector<MasterInst> showDialog(JFrame frame, JComponent location_comp, MasterMap masters)
	{
		InstanceDialogBox dialog = new InstanceDialogBox(frame, location_comp, masters);
		return dialog.getNewInsts();
	}
	
	/**
	 * Returns whether the inputs are valid, throws up dialogs to warn the user
	 */
	private boolean checkInputs()
	{
		// Text boxes
		if(text_instance_name.getText().isEmpty())
		{
			JOptionPane.showMessageDialog(this, "Instance name cannot be empty.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		if (!check_atomic.isSelected() && (combo_instance_master.getSelectedIndex() == -1))
		{
			JOptionPane.showMessageDialog(this, "You must specify an instance master for a non-atomic instance.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		
		// Check number formatting
		try
		{
			Double.parseDouble(text_x_position.getText());
			Double.parseDouble(text_y_position.getText());
			if (check_atomic.isSelected())
			{
				Double.parseDouble(text_width.getText());
				Double.parseDouble(text_height.getText());
			}
		}
		catch (NumberFormatException e)
		{
			JOptionPane.showMessageDialog(this, "Invalid numeric values used for position/dimension.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		try
		{
			if (check_array.isSelected())
			{
				Integer.parseInt(text_num_x.getText());
				Integer.parseInt(text_num_y.getText());
				Double.parseDouble(text_dx.getText());
				Double.parseDouble(text_dy.getText());
			}
		}
		catch (NumberFormatException e)
		{
			JOptionPane.showMessageDialog(this, "Invalid numeric values used for array parameters.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}

		return true;
	}
	
	/**
	 * Gets the new instances it added
	 */
	public Vector<MasterInst> getNewInsts() { return new_insts; }

}

class InstanceDialogButtonListener extends EventsHelper<InstanceDialogBox> implements ActionListener
{
	InstanceDialogButtonListener(InstanceDialogBox owner) 
	{
		super(owner);
	}

	public void actionPerformed(ActionEvent e) 
	{
		if (e.getActionCommand().equals("Ok"))
			owner.setVisible(!owner.createInstances());
	}
	
}

class InstanceAtomicListener extends EventsHelper<InstanceDialogBox> implements ItemListener
{
	InstanceAtomicListener(InstanceDialogBox owner) 
	{
		super(owner);
	}

	public void itemStateChanged(ItemEvent e)
	{
		if(e.getStateChange() == ItemEvent.DESELECTED)
			owner.setEditAtomic(false);
		else if (e.getStateChange() == ItemEvent.SELECTED)
			owner.setEditAtomic(true);
	}
}

class InstanceArrayListener extends EventsHelper<InstanceDialogBox> implements ItemListener
{
	InstanceArrayListener(InstanceDialogBox owner) 
	{
		super(owner);
	}
	
	public void itemStateChanged(ItemEvent e)
	{
		if(e.getStateChange() == ItemEvent.DESELECTED)
			owner.setEditArray(false);
		else if (e.getStateChange() == ItemEvent.SELECTED)
			owner.setEditArray(true);
	}
}

