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

import floorplan.FillerTree;
import floorplan.GridPoint;
import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;
import javax.swing.JProgressBar;
import javax.swing.SwingConstants;

public class FillerDialogBox extends JDialog
{
	// Buttons
	private JButton button_ok;
	private JButton button_cancel;
	private JButton button_calculate;
	
	// Text boxes
	private JTextField text_filler_instance_name;
	private JTextField text_filler_master_name;
	private JTextField text_max_aspect_ratio;

	// Labels
	private JLabel label_filler_count;
	
	// Instance of the filler master to return
	private MasterInst filler_inst;
	
	// Current master
	private Master cur_master;

		
	private FillerDialogBox(Floorplanner gui, MasterMap masters, Master cur_master) 
	{
		super(gui, "Generate Fillers", true);
		
		this.cur_master = cur_master;
		
		filler_inst = null;

		layoutGUI();
		addListeners();
		
		pack();
		setLocationRelativeTo(gui.getContentPane());
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
		gbl_panel_2.columnWidths = new int[]{10, 0, 49, 50, 0, 10};
		gbl_panel_2.rowHeights = new int[]{19, 0, 0, 0, 0, 0, 0};
		gbl_panel_2.columnWeights = new double[]{0.0, 1.0, 1.0, 0.0, 0.0, Double.MIN_VALUE};
		gbl_panel_2.rowWeights = new double[]{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		panel_2.setLayout(gbl_panel_2);
		
		JLabel label_filler_instance_name = new JLabel("Filler Instance Name");
		GridBagConstraints gbc_label_filler_instance_name = new GridBagConstraints();
		gbc_label_filler_instance_name.anchor = GridBagConstraints.EAST;
		gbc_label_filler_instance_name.insets = new Insets(0, 0, 5, 5);
		gbc_label_filler_instance_name.gridx = 1;
		gbc_label_filler_instance_name.gridy = 1;
		panel_2.add(label_filler_instance_name, gbc_label_filler_instance_name);
		
		text_filler_instance_name = new JTextField();
		GridBagConstraints gbc_text_filler_instance_name = new GridBagConstraints();
		gbc_text_filler_instance_name.gridwidth = 2;
		gbc_text_filler_instance_name.insets = new Insets(0, 0, 5, 5);
		gbc_text_filler_instance_name.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_filler_instance_name.gridx = 2;
		gbc_text_filler_instance_name.gridy = 1;
		panel_2.add(text_filler_instance_name, gbc_text_filler_instance_name);
		text_filler_instance_name.setColumns(10);
		text_filler_instance_name.setText("Filler");
		
		JLabel label_filler_master_name = new JLabel("Filler Master Name");
		GridBagConstraints gbc_label_filler_master_name = new GridBagConstraints();
		gbc_label_filler_master_name.anchor = GridBagConstraints.EAST;
		gbc_label_filler_master_name.insets = new Insets(0, 0, 5, 5);
		gbc_label_filler_master_name.gridx = 1;
		gbc_label_filler_master_name.gridy = 2;
		panel_2.add(label_filler_master_name, gbc_label_filler_master_name);
		
		text_filler_master_name = new JTextField();
		GridBagConstraints gbc_text_filler_master_name = new GridBagConstraints();
		gbc_text_filler_master_name.gridwidth = 2;
		gbc_text_filler_master_name.insets = new Insets(0, 0, 5, 5);
		gbc_text_filler_master_name.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_filler_master_name.gridx = 2;
		gbc_text_filler_master_name.gridy = 2;
		panel_2.add(text_filler_master_name, gbc_text_filler_master_name);
		text_filler_master_name.setColumns(10);
		text_filler_master_name.setText(cur_master.getName() + "_fill");
		
		JLabel label_max_aspect_ratio = new JLabel("Max Aspect Ratio");
		GridBagConstraints gbc_label_max_aspect_ratio = new GridBagConstraints();
		gbc_label_max_aspect_ratio.anchor = GridBagConstraints.EAST;
		gbc_label_max_aspect_ratio.insets = new Insets(0, 0, 5, 5);
		gbc_label_max_aspect_ratio.gridx = 1;
		gbc_label_max_aspect_ratio.gridy = 3;
		panel_2.add(label_max_aspect_ratio, gbc_label_max_aspect_ratio);
		
		text_max_aspect_ratio = new JTextField();
		GridBagConstraints gbc_text_max_aspect_ratio = new GridBagConstraints();
		gbc_text_max_aspect_ratio.insets = new Insets(0, 0, 5, 5);
		gbc_text_max_aspect_ratio.fill = GridBagConstraints.HORIZONTAL;
		gbc_text_max_aspect_ratio.gridx = 2;
		gbc_text_max_aspect_ratio.gridy = 3;
		panel_2.add(text_max_aspect_ratio, gbc_text_max_aspect_ratio);
		text_max_aspect_ratio.setColumns(10);
		text_max_aspect_ratio.setText(String.valueOf(FillerTree.MINIMUM_MAX_ASPECT_RATIO));
		
		button_calculate = new JButton("Calculate");
		GridBagConstraints gbc_button_calculate = new GridBagConstraints();
		gbc_button_calculate.insets = new Insets(0, 0, 5, 5);
		gbc_button_calculate.gridx = 3;
		gbc_button_calculate.gridy = 3;
		panel_2.add(button_calculate, gbc_button_calculate);
		
		label_filler_count = new JLabel("Fillers not Calculated");
		GridBagConstraints gbc_label_filler_count = new GridBagConstraints();
		gbc_label_filler_count.gridwidth = 3;
		gbc_label_filler_count.insets = new Insets(0, 0, 5, 5);
		gbc_label_filler_count.gridx = 1;
		gbc_label_filler_count.gridy = 4;
		panel_2.add(label_filler_count, gbc_label_filler_count);
	}
	
	public boolean createFillerInstance()
	{
		filler_inst = calcFillerInstance();
		return (filler_inst != null);
	}
	
	public MasterInst calcFillerInstance()
	{
		if (checkInputs())
		{
			label_filler_count.setText("Calculating filler instances");
			Master fill_master = Master.createFillerMaster(cur_master, 
					Double.parseDouble(text_max_aspect_ratio.getText()), text_filler_master_name.getText());
			MasterInst filler_inst = new MasterInst(cur_master, fill_master,
					text_filler_instance_name.getText(), GridPoint.ZERO, GridPoint.ZERO);
			label_filler_count.setText(fill_master.getInstances().size() + " filler instances");
			return filler_inst;
		}
		return null;
	}
	
	/**
	 * Adds listeners to various GUI elements
	 */
	private void addListeners()
	{
		button_ok.addActionListener(new FillerDialogButtonListener(this));
		button_cancel.addActionListener(new FillerDialogButtonListener(this));
		button_calculate.addActionListener(new FillerDialogButtonListener(this));
	}

	/**
	 * Show the dialog and returns a vector of the new instances to add
	 */
	public static void showDialog(Floorplanner gui, MasterMap masters, Master cur_master)
	{
		FillerDialogBox dialog = new FillerDialogBox(gui, masters, cur_master);
		
		MasterInst fill_inst = dialog.getFillerInst();
		if(fill_inst != null)
		{
			try
			{
				masters.addMaster(fill_inst.m);
				cur_master.addMasterInst(fill_inst);
				gui.updateMasters();
				gui.updateView(cur_master);
			}
			catch (Exception ex)
			{
				JOptionPane.showMessageDialog(gui, "Fill generation failed: " + ex.getMessage(),
						"Filler generation", JOptionPane.WARNING_MESSAGE);
			}
		}
	}
	
	/**
	 * Returns whether the inputs are valid, throws up dialogs to warn the user
	 */
	private boolean checkInputs()
	{
		double max_aspect_ratio = 3;

		// Text boxes
		if(text_filler_instance_name.getText().isEmpty())
		{
			JOptionPane.showMessageDialog(this, "Filler instance name cannot be empty.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}		
		if(text_filler_master_name.getText().isEmpty())
		{
			JOptionPane.showMessageDialog(this, "Filler master name cannot be empty.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		
		try
		{
			max_aspect_ratio = Double.parseDouble(text_max_aspect_ratio.getText());			
		}
		catch (NumberFormatException e)
		{
			JOptionPane.showMessageDialog(this, "Invalid number for max aspect ratio.",
					"Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}
		
		if (max_aspect_ratio < FillerTree.MINIMUM_MAX_ASPECT_RATIO)
		{
			JOptionPane.showMessageDialog(this, "Max aspect ratio must be greater than " + 
					FillerTree.MINIMUM_MAX_ASPECT_RATIO, "Error", JOptionPane.WARNING_MESSAGE);
			return false;
		}

		return true;
	}
	
	/**
	 * Gets the new instances it added
	 */
	public MasterInst getFillerInst() { return filler_inst; }

}

class FillerDialogButtonListener extends EventsHelper<FillerDialogBox> implements ActionListener
{
	FillerDialogButtonListener(FillerDialogBox owner) 
	{
		super(owner);
	}

	public void actionPerformed(ActionEvent e) 
	{
		if (e.getActionCommand().equals("Ok"))
			owner.setVisible(!owner.createFillerInstance());
		else if (e.getActionCommand().equals("Cancel"))
			owner.setVisible(false);
		else if (e.getActionCommand().equals("Calculate"))
			owner.calcFillerInstance();
	}
	
}


