package gui;

import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.awt.Dimension;

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
import javax.swing.JTabbedPane;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeSelectionModel;

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
	private JComboBox<Master> masters_box;
	
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
		JLabel lblSelectedMaster = new JLabel("Selected Master");
		panel_2.add(lblSelectedMaster);
		masters_box = new JComboBox<Master>();
		masters_box.setPreferredSize(new Dimension(200, 20));
		masters_box.addActionListener(new MasterBoxListener(this));
		panel_2.add(masters_box);
		
		MasterButtonListener button_listener = new MasterButtonListener(this);
		
		JButton button_view = new JButton("View");
		button_view.addActionListener(button_listener);
		panel_1.add(button_view);
		
		JButton button_new = new JButton("New");
		button_new.addActionListener(button_listener);
		panel_1.add(button_new);
		
		JButton button_rename = new JButton("Rename");
		button_rename.addActionListener(button_listener);
		panel_1.add(button_rename);
		
		JButton button_delete = new JButton("Delete");
		button_delete.addActionListener(button_listener);
		panel_1.add(button_delete);
		
		
		JTabbedPane view_tabbed_pane = new JTabbedPane(JTabbedPane.TOP);
		view_tabbed_pane.setPreferredSize(new Dimension(300, 550));
		add(view_tabbed_pane, BorderLayout.CENTER);
		
		hier_tree = new JTree();
		hier_tree.setShowsRootHandles(true);
		hier_tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);		
//		hier_tree.getSelectionModel().addTreeSelectionListener(this);
//		new HierMouse(hier_tree);
		view_tabbed_pane.addTab("Hierarchy", null, hier_tree, null);
		JScrollPane hier_scroller = new JScrollPane(hier_tree);
		view_tabbed_pane.addTab("Hierarchy", null, hier_scroller, null);
		
		objects_table = new JTable();
		objects_table.setAutoCreateRowSorter(true);
		objects_table.getSelectionModel().addListSelectionListener(new ObjectsTableListener(this));
		view_tabbed_pane.addTab("Objects", null, objects_table, null);
		JScrollPane objects_scroller = new JScrollPane(objects_table);
		view_tabbed_pane.addTab("Objects", null, objects_scroller, null);	
	}
	
	/**
	 * Called by the GUI whenever the master map changes
	 */
	public void updateMasters(MasterMap masters)
	{
		masters_box.setModel(masters);
		masters_box.setSelectedItem(masters.getDefaultMaster());
	}
	
	/**
	 * Called by the GUI whenever the view is updated
	 */
	public void updateView(Master new_master)
	{
		objects_table.setModel(new_master);
		hier_tree.setModel(new DefaultTreeModel(new_master.getLibInstance()));
	}
	
	public Floorplanner getGUI() { return gui; }
	public JTree getHierTree() { return hier_tree; }
	public JTable getObjectsTable() { return objects_table; }
	public JComboBox<Master> getMastersBox() { return masters_box; }
}

class MasterButtonListener extends EventsHelper<ViewPanel> implements ActionListener
{
	MasterButtonListener(ViewPanel owner) 
	{
		super(owner);
	}

	@Override
	public void actionPerformed(ActionEvent e) 
	{
		try
		{
			if (e.getActionCommand() == "View")
				owner.getGUI().updateView((Master) owner.getMastersBox().getSelectedItem());
			else if (e.getActionCommand() == "New")
			{
				owner.getGUI().getMasters().addMaster();
				owner.getMastersBox().setModel(new DefaultComboBoxModel(owner.getGUI().getMasters().masters.values().toArray()));
				//owner.getGUI().updateView(owner.getGUI().getMasters().getDefaultMaster());
			}
			else if (e.getActionCommand() == "Rename")
			{}
			else if (e.getActionCommand() == "Delete")
			{
				owner.getGUI().getMasters().removeMaster(((Master) owner.getMastersBox().getSelectedItem()).getName());
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
		JComboBox<Master> master_box = (JComboBox<Master>) e.getSource();
		if (master_box.getSelectedItem() == null)
		{
			// Disable buttons
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