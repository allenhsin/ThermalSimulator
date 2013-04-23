package gui;

import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.EventQueue;

import javax.swing.JComboBox;
import java.awt.FlowLayout;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTree;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.util.Vector;

import javax.swing.JTabbedPane;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeSelectionModel;

import floorplan.InstanceTableModel;
import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;

public class ViewPanel extends JPanel
{
	// Reference to main floorplanner GUI
	private Floorplanner gui;
	// Hierarchy of the current master
	private JTree hier_tree;
	// Table of objects in the current master
	private JTable objects_table;
	// Combo box for selected masters
	private JComboBox masters_box;
	// Master control buttons
	private JButton button_new, button_rename, button_delete;
	// Instance control buttons
	private JButton button_new_instance, button_delete_instance;
	
	public ViewPanel(Floorplanner gui)
	{
		this.gui = gui;
		initialize();
	}
	
	private void initialize()
	{
		setLayout(new BorderLayout(0, 0));
		
		JPanel panel = new JPanel();
		add(panel, BorderLayout.NORTH);
		panel.setLayout(new BorderLayout(0, 0));
		
		JPanel panel_1 = new JPanel();
		panel.add(panel_1, BorderLayout.SOUTH);
		panel_1.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		JPanel panel_2 = new JPanel();
		panel.add(panel_2, BorderLayout.CENTER);
		panel_2.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));		
		JLabel label_master = new JLabel("Floorplan");
		panel_2.add(label_master);
		masters_box = new JComboBox();
		masters_box.setPreferredSize(new Dimension(180, 20));
		masters_box.addActionListener(new MasterBoxListener(this));	
		panel_2.add(masters_box);
		
		MasterButtonListener button_listener = new MasterButtonListener(this);
		
		button_new = new JButton("New");
		button_rename = new JButton("Rename");
		button_delete = new JButton("Delete");
		
		button_new.addActionListener(button_listener);
		button_rename.addActionListener(button_listener);
		button_delete.addActionListener(button_listener);

		panel_1.add(button_new);		
		panel_1.add(button_rename);		
		panel_1.add(button_delete);
		
		
		JTabbedPane view_tabbed_pane = new JTabbedPane(JTabbedPane.TOP);
		view_tabbed_pane.setPreferredSize(new Dimension(300, 550));
		add(view_tabbed_pane, BorderLayout.CENTER);
		
		JPanel panel_3 = new JPanel();
		panel_3.setLayout(new BorderLayout(0, 5));
		view_tabbed_pane.addTab("Instances", null, panel_3, null);
		
		JPanel instance_button_panel = new JPanel();
		instance_button_panel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		panel_3.add(instance_button_panel, BorderLayout.SOUTH);
		
		InstanceButtonListener inst_button_listener = new InstanceButtonListener(this);
		
		button_new_instance = new JButton("New Instance...");
		button_delete_instance = new JButton("Delete");

		button_new_instance.addActionListener(inst_button_listener);
		button_delete_instance.addActionListener(inst_button_listener);
		
		instance_button_panel.add(button_new_instance);
		instance_button_panel.add(button_delete_instance);
		
		objects_table = new JTable();
		objects_table.setAutoCreateRowSorter(true);
		objects_table.getSelectionModel().addListSelectionListener(new ObjectsTableListener(this));
		panel_3.add(objects_table, BorderLayout.CENTER);
		JScrollPane objects_scroller = new JScrollPane(objects_table);
		panel_3.add(objects_scroller, BorderLayout.CENTER);	
		
		hier_tree = new JTree();
		hier_tree.setShowsRootHandles(true);
		hier_tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);		
//		hier_tree.getSelectionModel().addTreeSelectionListener(this);
//		new HierMouse(hier_tree);
		view_tabbed_pane.addTab("Hierarchy", null, hier_tree, null);
		JScrollPane hier_scroller = new JScrollPane(hier_tree);
		view_tabbed_pane.addTab("Hierarchy", null, hier_scroller, null);
	}
	
	/**
	 * Called by the GUI whenever the master map changes
	 */
	public void updateMasters(MasterMap masters)
	{
		masters_box.setModel(new DefaultComboBoxModel(masters.toArray()));
		masters_box.setSelectedItem(masters.getDefaultMaster());
	}
	
	/**
	 * Called by the GUI whenever the view is updated
	 */
	public void updateView(Master new_master)
	{
		objects_table.setModel(new InstanceTableModel(new_master));
		if (new_master == null)
		{
			button_new_instance.setEnabled(false);
			button_delete_instance.setEnabled(false);
			hier_tree.setModel(new DefaultTreeModel(null));
		}
		else
		{
			hier_tree.setModel(new DefaultTreeModel(new_master.getLibInstance()));
			button_new_instance.setEnabled(true);
			button_delete_instance.setEnabled(true);
		}
	}
	
	public Floorplanner getGUI() { return gui; }
	public JTree getHierTree() { return hier_tree; }
	public JTable getObjectsTable() { return objects_table; }
	public JComboBox getMastersBox() { return masters_box; }
	public JButton getButtonNew() { return button_new; }
	public JButton getButtonRename() { return button_rename; }
	public JButton getButtonDelete() { return button_delete; }
}

class InstanceButtonListener extends EventsHelper<ViewPanel> implements ActionListener
{
	InstanceButtonListener(ViewPanel owner) 
	{
		super(owner);
	}

	public void actionPerformed(ActionEvent e) 
	{
		Master cur_master = owner.getGUI().getCurMaster();
		if (e.getActionCommand() == "New Instance...")
		{
			Vector<MasterInst> new_insts = InstanceDialogBox.showDialog(owner.getGUI(),
					owner.getGUI().getMasters(), owner.getGUI().getCurMaster());
			
			// Add and check to see if the additions succeeded
			boolean add_success = true;
			for (int i = 0; i < new_insts.size(); ++i)
				add_success &= cur_master.addMasterInst(new_insts.get(i));
			
			// Warn the user
			if(!add_success)
			{				
				JOptionPane.showMessageDialog(owner.getGUI(), "Some instance(s) were not added: new instance(s) had instance name conflicts!",
						"Warning", JOptionPane.WARNING_MESSAGE);
			}
			
			// Check for recursive floorplans
			if (owner.getGUI().getMasters().hasRecursiveMasters(cur_master))
			{
				// If there are recursive floorplans, undo adds and throw exception
				for (int i = 0; i < new_insts.size(); ++i)
					cur_master.deleteMasterInst(new_insts.get(i));
				
				JOptionPane.showMessageDialog(owner.getGUI(), "Failed to add new instance(s): new instance(s) causes recursive floorplans!",
						"Operation failed", JOptionPane.WARNING_MESSAGE);
			}
			
			// Any changes causes temperature traces to be cleared
			if (!new_insts.isEmpty())
				owner.getGUI().getRenderPanel().unloadTempTraces();		
		}
		else if (e.getActionCommand() == "Delete")
		{
			JTable table = owner.getObjectsTable();
			int[] selected_rows = table.getSelectedRows();
			String[] selected_names = new String[selected_rows.length];
			for (int i = 0; i < selected_rows.length; ++i)
				selected_names[i] = (String) table.getValueAt(selected_rows[i],  0);
			
			for (int i = 0; i < selected_names.length; ++i)
				cur_master.deleteMasterInst(selected_names[i]);

			// Any changes causes temperature traces to be cleared
			if (selected_names.length > 0)
				owner.getGUI().getRenderPanel().unloadTempTraces();		
		}
		owner.getGUI().updateView();
	}
}

class MasterButtonListener extends EventsHelper<ViewPanel> implements ActionListener
{
	MasterButtonListener(ViewPanel owner) 
	{
		super(owner);
	}

	public void actionPerformed(ActionEvent e) 
	{
		JComboBox master_box = owner.getMastersBox();
		Master selected_master = (Master) master_box.getSelectedItem();
		MasterMap masters = owner.getGUI().getMasters();
		
		try
		{
			if (e.getActionCommand() == "New")
			{
				masters.addMaster();
				owner.getGUI().updateMasters();
			}
			else if (e.getActionCommand() == "Rename")
			{
				String new_name = (String) JOptionPane.showInputDialog(owner.getGUI(),
	                    "Enter the new name of the master", "Rename master",
	                    JOptionPane.PLAIN_MESSAGE, null, null,
	                    selected_master.getName());
				if (new_name != null)
				{
					masters.renameMaster(selected_master.getName(), new_name);
					owner.getGUI().updateMasters();
				}
			}
			else if (e.getActionCommand() == "Delete")
			{
				int decision = JOptionPane.showConfirmDialog(owner.getGUI(), "Confirm deletion of floorplan master '" +
						selected_master.getName() + "' ?", "Confirm deletion", JOptionPane.YES_NO_OPTION);
				if (decision == JOptionPane.YES_OPTION)
				{
					masters.removeMaster(selected_master);
					owner.getGUI().updateMasters();
				}
			}
			else throw new Error("Internal Error: Button '" + e.toString() + "' is not supported!");
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(owner.getGUI(), ex.getMessage(), "Operation failed", JOptionPane.WARNING_MESSAGE);
		}
	}
}

class MasterBoxListener extends EventsHelper<ViewPanel> implements ActionListener
{
	MasterBoxListener(ViewPanel owner)
	{
		super(owner);
	}
	
	public void actionPerformed(ActionEvent e) 
	{
		JComboBox master_box = (JComboBox) e.getSource();
		if (master_box.getSelectedItem() == null)
		{
			owner.getButtonDelete().setEnabled(false);
			owner.getButtonRename().setEnabled(false);
		}
		else
		{
			owner.getButtonDelete().setEnabled(true);
			owner.getButtonRename().setEnabled(true);
			owner.getGUI().updateView((Master) master_box.getSelectedItem());
		}
	}	
}

class HierTreeListener extends EventsHelper<ViewPanel> implements TreeSelectionListener
{
	
	HierTreeListener(ViewPanel owner) 
	{
		super(owner);
	}

	public void valueChanged(TreeSelectionEvent arg0)
	{
//		if (view_panel.getHierTree().getLastSelectedPathComponent() != null)
//		{
//			if (view_panel.getHierTree().getLastSelectedPathComponent() != masters)
//				updateView(((MasterInst) view_panel.getHierTree().getLastSelectedPathComponent()));
//		}
	
	}
}

class ObjectsTableListener extends EventsHelper<ViewPanel> implements ListSelectionListener 
{
	ObjectsTableListener(ViewPanel owner)
	{
		super(owner);
	}

	public void valueChanged(ListSelectionEvent e)
	{
		owner.getGUI().updateHighlights();
	}
}