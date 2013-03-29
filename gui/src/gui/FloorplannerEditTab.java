package gui;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JOptionPane;
import javax.swing.JTabbedPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;

import java.awt.GridBagLayout;
import javax.swing.JLabel;
import java.awt.GridBagConstraints;
import javax.swing.JButton;
import java.awt.Insets;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import floorplan.InstanceTableModel;
import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.tree.DefaultTreeModel;

public class FloorplannerEditTab extends JPanel implements ListSelectionListener, ActionListener, ItemListener
{
	// Reference to the main floorplanner GUI
	private Floorplanner gui;
	
	// Buttons
	private JButton button_previous;
	private JButton button_next;
	private JButton button_apply;
	private JButton button_discard;
	
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
	
	// Object number label
	private JLabel label_object_num;
	// Modified or not label
	private JLabel label_mod;
	
	// List of selected master instances
	private List<MasterInst> selected;
	// Current selection index
	private int sel_index;
	// If there are currently unapplied changes
	private boolean changed;
	
	
	/**
	 * Create a tool tab with the correct tab location
	 */
	FloorplannerEditTab(Floorplanner gui)
	{
		super();
		this.gui = gui;
		gui.getObjectsTable().getSelectionModel().addListSelectionListener(this);
		selected = new LinkedList<MasterInst>();
		createEditTab();
	}
	
	public void updateMasters(MasterMap new_masters)
	{
		combo_instance_master.setModel(new DefaultComboBoxModel(new_masters.toArray()));
	}
	
	private void createEditTab()
	{
		setLayout(new BorderLayout(0, 0));
		
		JPanel panel_1 = new JPanel();
		add(panel_1, BorderLayout.NORTH);
		panel_1.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		button_apply = new JButton("Apply");
		button_apply.addActionListener(this);
		panel_1.add(button_apply);
		
		button_discard = new JButton("Discard");
		button_discard.addActionListener(this);
		panel_1.add(button_discard);
		
		JLabel label_object = new JLabel("Object");
		panel_1.add(label_object);
		
		label_object_num = new JLabel("0/0");
		panel_1.add(label_object_num);
		
		button_previous = new JButton("<-");
		button_previous.addActionListener(this);
		panel_1.add(button_previous);
		
		button_next = new JButton("->");
		button_next.addActionListener(this);
		panel_1.add(button_next);
		
		JPanel panel_2 = new JPanel();
		add(panel_2, BorderLayout.CENTER);
		GridBagLayout gbl_panel_2 = new GridBagLayout();
		gbl_panel_2.columnWidths = new int[]{10, 0, 100, 50, 0, 0, 10};
		gbl_panel_2.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		gbl_panel_2.columnWeights = new double[]{0.0, 0.0, 1.0, 1.0, 0.0, 0.0, Double.MIN_VALUE};
		gbl_panel_2.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
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
		text_instance_name.getDocument().addDocumentListener(new TextBoxEvents(this));
		
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
		panel_2.add(combo_instance_master, gbc_combo_instance_master);
		combo_instance_master.addActionListener(this);
		
		check_atomic = new JCheckBox("Atomic");
		check_atomic.addItemListener(this);
		check_atomic.setEnabled(false);
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
		text_x_position.getDocument().addDocumentListener(new TextBoxEvents(this));
		
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
		text_y_position.getDocument().addDocumentListener(new TextBoxEvents(this));
		
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
		text_width.getDocument().addDocumentListener(new TextBoxEvents(this));
		
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
		text_height.getDocument().addDocumentListener(new TextBoxEvents(this));
		
		label_mod = new JLabel("No modifications made");
		GridBagConstraints gbc_label_mod = new GridBagConstraints();
		gbc_label_mod.gridwidth = 2;
		gbc_label_mod.insets = new Insets(0, 0, 0, 5);
		gbc_label_mod.gridx = 2;
		gbc_label_mod.gridy = 8;
		panel_2.add(label_mod, gbc_label_mod);
		
		updateSelected(0);
	}

	/**
	 * When things get selected, change what is currently displayed in the edit tab
	 * and also prompt user to save current changes if there changes
	 */
	public void valueChanged(ListSelectionEvent e) 
	{
		// Before doing anything, ask user to save changes, if there were modifications
		checkApply();

		ListSelectionModel lsm = (ListSelectionModel) e.getSource();
		Master cur_master = gui.getCurMaster();
		
		if (!e.getValueIsAdjusting())
		{
			selected.clear();
			
			if(!lsm.isSelectionEmpty())
			{
				for (int i = lsm.getMinSelectionIndex(); i <= lsm.getMaxSelectionIndex(); ++i)
				{
					if (lsm.isSelectedIndex(i))
					{
						// Get the floorplan that is being edited
						MasterInst edit_inst = cur_master.getInstances().get(i);
						selected.add(edit_inst);
					}
				}			
			}
		}
		updateSelected(0);
	}
	
	// Tell the edit tab to update the display for the currently selected items
	private void updateSelected(int sel_index)
	{
		// Then change the selection
		this.sel_index = sel_index;
		// Disable buttons as appropriate
		if (sel_index > 0) button_previous.setEnabled(true);
		else button_previous.setEnabled(false);
		if (sel_index < selected.size() - 1) button_next.setEnabled(true);
		else button_next.setEnabled(false);
		
		// If no objects are selected
		if (selected.size() == 0)
		{
			text_instance_name.setEnabled(false);
			text_x_position.setEnabled(false);
			text_y_position.setEnabled(false);
			text_height.setEnabled(false);
			text_width.setEnabled(false);
			check_atomic.setEnabled(false);
			combo_instance_master.setEnabled(false);

			label_object_num.setText("0/0");
			text_instance_name.setText("");
			text_x_position.setText("");
			text_y_position.setText("");
			text_height.setText("");
			text_width.setText("");
		}
		else
		{
			label_object_num.setText((sel_index + 1) + "/" + selected.size());		
			// Update the text boxes with the parameters
			MasterInst cur_inst = selected.get(sel_index);
			
			text_instance_name.setEnabled(true);
			text_x_position.setEnabled(true);
			text_y_position.setEnabled(true);
			check_atomic.setEnabled(true);
			
			text_instance_name.setText(cur_inst.n);
			text_x_position.setText(Double.toString(cur_inst.x));
			text_y_position.setText(Double.toString(cur_inst.y));
			
			check_atomic.setSelected(cur_inst.m.isAtomic());
			setEditAtomic(cur_inst.m.isAtomic());
			text_height.setText(Double.toString(cur_inst.m.getHeight()));
			text_width.setText(Double.toString(cur_inst.m.getWidth()));
			combo_instance_master.setSelectedItem(cur_inst.m);
		}

		// Fresh copy, no changes
		setChanges(false);		
	}
	
	/**
	 * Set the state of the appropriate text boxes for an atomic/non-atomic instances
	 */
	private void setEditAtomic(boolean atomic)
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
	 * Pushes the changes to the master
	 */
	private void applySelected()
	{	
		// get the selected instance
		MasterInst edit_master_inst = selected.get(sel_index);

		// Edit the instantiation
		edit_master_inst.x = Double.parseDouble(text_x_position.getText());
		edit_master_inst.y = Double.parseDouble(text_y_position.getText());
		edit_master_inst.n = text_instance_name.getText();
		
		// If atomic is selected, make the thing a new atomic!
		if (check_atomic.isSelected())
			edit_master_inst.m = new Master(Double.parseDouble(text_width.getText()), Double.parseDouble(text_height.getText()));
		// If the atomic is not selected then find the master and set it to that
		else
		{
			Master old_master = edit_master_inst.m;
			Master new_master = (Master) combo_instance_master.getSelectedItem();
			edit_master_inst.m = new_master;
			if (gui.getMasters().hasRecursiveMasters())
			{
				combo_instance_master.setSelectedItem(old_master);
				check_atomic.setSelected(old_master.isAtomic());
				edit_master_inst.m = old_master;
				JOptionPane.showMessageDialog(gui, "Changing instance master to " + new_master.getName() + " causes recursive floorplans! Instance master will not be changed.",
						"Error", JOptionPane.WARNING_MESSAGE);
			}
		}		
		
		// Fresh copy, no changes
		setChanges(false);
		gui.refreshView();
	}
	
	/**
	 * Set that the selection has been changed
	 */
	protected void setChanges(boolean changed)
	{
		this.changed = changed;
		button_discard.setEnabled(changed);
		button_apply.setEnabled(changed);
		if (changed) label_mod.setText("Modifications made");
		else label_mod.setText("No modifications made");
	}
	
	/**
	 * Handle actions for all the buttons
	 */
	public void actionPerformed(ActionEvent e) 
	{
		if (e.getSource() == combo_instance_master)
			setChanges(true);
		else if (e.getSource() == button_next)
		{
			checkApply();
			updateSelected(sel_index + 1);
		}
		else if (e.getSource() == button_previous)
		{
			checkApply();
			updateSelected(sel_index - 1);
		}
		else if (e.getSource() == button_apply)
			applySelected();
		else if (e.getSource() == button_discard)
			updateSelected(sel_index);
		else throw new Error("Internal Error: " + e.getActionCommand() + "' is not supported!");
	}

	/**
	 * Handles state changes for the atomic checkbox
	 */
	public void itemStateChanged(ItemEvent e)
	{
		setChanges(true);
		if (e.getSource() == check_atomic)
		{
			if(e.getStateChange() == ItemEvent.DESELECTED)
				setEditAtomic(false);
			else if (e.getStateChange() == ItemEvent.SELECTED)
				setEditAtomic(true);
		}
		else throw new Error("Internal Error: Button '" + e.toString() + "' is not supported!");
	}
	
	/**
	 * Checks to see if there have been changes and forces the user to apply
	 * or discard current changes if there have been changes
	 */
	private void checkApply()
	{
		if (changed)
		{
			int sel = JOptionPane.showOptionDialog(gui, "Current object has modifications, apply modifications?",
				"Apply modifications", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE,
				null, new String[] {"Apply", "Discard"}, "Discard");
			if (sel == 0) applySelected();
			else if (sel == 1) updateSelected(sel_index);
		}
	}

}

/**
 *  Lots of helper classes for handling of events
 */

/**
 *  Handles changes to the text boxes 
 */
class TextBoxEvents extends EventsHelper<FloorplannerEditTab> implements DocumentListener
{

	TextBoxEvents(FloorplannerEditTab edit_tab) { super(edit_tab); }
	public void changedUpdate(DocumentEvent arg0) { owner.setChanges(true); }
	public void insertUpdate(DocumentEvent arg0) { owner.setChanges(true); }
	public void removeUpdate(DocumentEvent arg0) { owner.setChanges(true); }
	
}

