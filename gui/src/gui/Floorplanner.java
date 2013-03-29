package gui;

import java.awt.Dimension;

import javax.swing.JFrame;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import display.RenderPanel;
import floorplan.FloorplanParser;
import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;

import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.table.DefaultTableModel;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import java.io.File;
import java.util.HashMap;

import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.ListSelectionModel;

import temperature.TemperatureTrace;

public class Floorplanner extends JFrame implements ListSelectionListener, TreeSelectionListener
{	
	// The master map containing all masters
	private MasterMap masters;
	// The currently displayed Master
	private Master cur_master;

	// The tool tab
	private FloorplannerTools tools;
	
	private RenderPanel render_panel;
	private FloorplannerMenu menu_bar;
	
	// View panel
	private ViewPanel view_panel;
	
	public Floorplanner (String title, Dimension render_size) 
	{
		super (title);	
		
		initialize(render_size);		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		this.pack();
		setResizable(true);
		setVisible (true);
		
		createNewFloorplan();
	}
	
	public void createNewFloorplan()
	{
		MasterMap new_masters = new MasterMap();
		updateMasters(new_masters);
	}
	
	public void openFloorplanFile(File fplan_file)
	{
		try
		{
			updateMasters(FloorplanParser.parseMasters(fplan_file));
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(this, ex.getMessage(), "Open", JOptionPane.WARNING_MESSAGE);
		}
	}

	public void openTemperatureTrace(File temp_trace)
	{
		try
		{
			render_panel.setTempTrace(TemperatureTrace.parseTemperatureTrace(temp_trace));
		}
		catch (Exception e)
		{ 
			JOptionPane.showMessageDialog(this, e.getMessage(), "Open", JOptionPane.WARNING_MESSAGE);
		}
	}
	
	private void initialize(Dimension render_size)
	{
		setLayout(new BorderLayout());
		menu_bar = new FloorplannerMenu(this);
		setJMenuBar(menu_bar);

		view_panel = new ViewPanel(this);
		getContentPane().add(view_panel, BorderLayout.WEST);
			
		render_panel = new RenderPanel(render_size);
		getContentPane().add(render_panel, BorderLayout.CENTER);
		
		tools = new FloorplannerTools(this);
		getContentPane().add(tools, BorderLayout.EAST);
	}
	
	
	/**
	 * Updates the current masters map, should be called whenever the main master map changes
	 * as this function will contain bookkeeping calls to all other GUI elements 
	 */
	public void updateMasters() { updateMasters(masters); }
	public void updateMasters(MasterMap new_masters)
	{
		masters = new_masters;
		tools.updateMasters(new_masters);
		view_panel.updateMasters(new_masters);
		
		// Update to the default view of the new masters map does not
		// contain the current displayed masters
		if (new_masters != null && cur_master != null &&
			masters.getMasters().containsKey(cur_master.getName()))
			updateView(masters.getMasters().get(cur_master.getName()));
		else
			updateView(null);
	}
	
	/**
	 * Refreshes the view. Should be called if instance properties change, but
	 * no instances have been added or deleted
	 */
	public void refreshView()
	{
		// Just needs to repaint, nothing else
		updateHighlights();
		render_panel.repaint();
		view_panel.repaint();
	}
	
	/**
	 * Updates the current view to a view of the new master. Should be
	 * called if there has been changes to what master to view
	 */
	public void updateView() { updateView(cur_master); }
	public void updateView(Master new_master)
	{
		if (new_master == null)
			cur_master = masters.getDefaultMaster();
		else
			cur_master = new_master;
		view_panel.updateView(cur_master);
		render_panel.setView(cur_master);
		refreshView();
	}
	
	/**
	 * Update all the highlights all the highlights
	 */
	public void updateHighlights()
	{
		ListSelectionModel lsm = view_panel.getObjectsTable().getSelectionModel();
		HashMap<String, Boolean> highlights = render_panel.getRender().getHighlights();
		
		// Clear all current highlights
		highlights.clear();
		// Check if selection is empty
		if (!lsm.isSelectionEmpty())
		{
			// Go through all selected entries, add to table
			for (int i = lsm.getMinSelectionIndex(); i <= lsm.getMaxSelectionIndex(); ++i)
			{
				// If it has been selected, add it to the highlights
				if (lsm.isSelectedIndex(i))
					highlights.put((String) view_panel.getObjectsTable().getValueAt(i, 0), new Boolean(true));
			}
		}
		render_panel.repaint();
	}
	
	public void valueChanged(ListSelectionEvent e)
	{
		updateHighlights();
		render_panel.repaint();
//		ListSelectionModel lsm = (ListSelectionModel) e.getSource();
//		HashMap<String, Boolean> highlights = render_panel.getRender().getHighlights();
//		
//		for (int i = e.getFirstIndex(); i <= e.getLastIndex(); ++i)
//		{
//			// Only need to do this if i is within the table row range
//			if (i >= 0 && i < objects_table.getModel().getRowCount())
//			{
//				// If it has been selected, add it to the highlights
//				if (lsm.isSelectedIndex(i))
//					highlights.put((String) objects_table.getValueAt(i, 0), new Boolean(true));
//				// If it has been deselected, remove it from the highlights
//				else
//					highlights.remove((String) objects_table.getValueAt(i, 0));
//			}
//		}		
//		render_panel.repaint();
	}
	
	public void valueChanged(TreeSelectionEvent e) 
	{
//		if (view_panel.getHierTree().getLastSelectedPathComponent() != null)
//		{
//			if (view_panel.getHierTree().getLastSelectedPathComponent() != masters)
//				updateView(((MasterInst) view_panel.getHierTree().getLastSelectedPathComponent()));
//		}
	}
	
	public Master getCurMaster() { return cur_master; }
	public MasterMap getMasters() { return masters; }
	public JTable getObjectsTable() { return view_panel.getObjectsTable(); }
	public RenderPanel getRenderPanel() { return render_panel; }
	
}

class HierMouse extends MouseAdapter
{
	private JTree hier_tree;
	
	HierMouse (JTree hier_tree)
	{
		this.hier_tree = hier_tree;
		hier_tree.addMouseListener(this);
	}
	public void mouseClicked(MouseEvent e)
	{
		if (e.getButton() == MouseEvent.BUTTON3)
		{
			TreePath sel_path = hier_tree.getPathForLocation(e.getX(), e.getY());
			hier_tree.setSelectionPath(sel_path);
			if (!((MasterInst) sel_path.getLastPathComponent()).isAtomic())
			{
				JPopupMenu popup = new JPopupMenu();
				JMenuItem rename = new JMenuItem("Rename");
				JMenuItem remove = new JMenuItem("Delete");
				rename.addActionListener(new HierContext(hier_tree));
				remove.addActionListener(new HierContext(hier_tree));
				popup.add(rename);
				popup.add(remove);
				popup.show(hier_tree, e.getX(), e.getY());
			}
		}
	}
}

class HierContext implements ActionListener
{
	private JTree hier_tree;
	HierContext(JTree hier_tree)
	{
		this.hier_tree = hier_tree;
	}
	public void actionPerformed(ActionEvent e) 
	{
		try
		{
			MasterInst t = (MasterInst) hier_tree.getSelectionPath().getLastPathComponent();
			
			if (e.getActionCommand().equals("Rename"))
			{
				((MasterMap) hier_tree.getModel().getRoot()).renameMaster(
						t.m.getName(), "BLAH");			
			}
			else if (e.getActionCommand().equals("Delete"))
			{
				((MasterMap) hier_tree.getModel().getRoot()).removeMaster(
						t.m.getName());
				((DefaultTreeModel) hier_tree.getModel()).nodeStructureChanged(t);
			}
		}
		catch (Exception ex)
		{
			JOptionPane.showMessageDialog(hier_tree, ex.getMessage(), "Open", JOptionPane.WARNING_MESSAGE);
		}
	}
}

