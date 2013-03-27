package gui;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import javax.swing.JLabel;
import display.FloorplanRender;
import display.RenderPanel;
import floorplan.Floorplan;
import floorplan.FloorplanParser;
import floorplan.Master;
import floorplan.MasterInst;
import floorplan.MasterMap;

import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.table.DefaultTableModel;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeSelectionModel;

import java.io.File;
import java.util.HashMap;

import javax.swing.JOptionPane;
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
	private MasterInst cur_inst;

	// The tool tab
	private FloorplannerTools tools;
	
	private RenderPanel render_panel;
	private ViewTab view_tabbed_pane;
	private FloorplannerMenu menu_bar;
	
	private JTable objects_table;
	private JTree hier_tree;
	
	// Scrollers for the left-side tab
	private JScrollPane objects_scroller, hier_scroller;	
	
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

		view_tabbed_pane = new ViewTab(JTabbedPane.TOP);
		view_tabbed_pane.setPreferredSize(new Dimension(350, 550));
		getContentPane().add(view_tabbed_pane, BorderLayout.WEST);
		
		hier_tree = new JTree();
		hier_tree.setRootVisible(false);
		hier_tree.setShowsRootHandles(true);
		hier_tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);		
		hier_tree.getSelectionModel().addTreeSelectionListener(this);
		view_tabbed_pane.addTab("Hierarchy", null, hier_tree, null);
		hier_scroller = new JScrollPane(hier_tree);
		view_tabbed_pane.addTab("Hierarchy", null, hier_scroller, null);
		
		objects_table = new JTable();
		objects_table.setAutoCreateRowSorter(true);
		objects_table.getSelectionModel().addListSelectionListener(this);
		view_tabbed_pane.addTab("Objects", null, objects_table, null);
		objects_scroller = new JScrollPane(objects_table);
		view_tabbed_pane.addTab("Objects", null, objects_scroller, null);
	
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
		hier_tree.setModel(new DefaultTreeModel(masters));
		updateView(cur_inst);
	}
	
	/**
	 * Refreshes the view
	 */
	public void refreshView()
	{
		updateView(cur_inst);
	}
	
	
	/**
	 * Updates the current view to a view of the new master instance
	 */
	public void updateView(MasterInst new_inst)
	{
		if (new_inst == null)
			cur_inst = new MasterInst(masters.getDefaultMaster(), "Top", 0.0, 0.0);
		else
			cur_inst = new_inst;		
		objects_table.setModel(cur_inst.m);
		render_panel.setView(cur_inst);
		render_panel.repaint();
	}
	
	public void valueChanged(ListSelectionEvent e)
	{
		ListSelectionModel lsm = (ListSelectionModel) e.getSource();
		HashMap<String, Boolean> highlights = render_panel.getRender().getHighlights();
		
		for (int i = e.getFirstIndex(); i <= e.getLastIndex(); ++i)
		{
			// Only need to do this if i is within the table row range
			if (i >= 0 && i < objects_table.getModel().getRowCount())
			{
				// If it has been selected, add it to the highlights
				if (lsm.isSelectedIndex(i))
					highlights.put((String) objects_table.getValueAt(i, 0), new Boolean(true));
				// If it has been deselected, remove it from the highlights
				else
					highlights.remove((String) objects_table.getValueAt(i, 0));
			}
		}		
		render_panel.repaint();
	}
	
	private void rehighlight()
	{
		
	}

	public void valueChanged(TreeSelectionEvent e) 
	{
		if (hier_tree.getLastSelectedPathComponent() != null)
		{
			if (hier_tree.getLastSelectedPathComponent() != masters)
				updateView(((MasterInst) hier_tree.getLastSelectedPathComponent()));
		}
	}
	
	public MasterInst getCurInst() { return cur_inst; }
	public MasterMap getMasters() { return masters; }
	public JTable getObjectsTable() { return objects_table; }
	public RenderPanel getRenderPanel() { return render_panel; }
	
}


class ViewTab extends JTabbedPane
{
	ViewTab (int tab_loc)
	{
		super(tab_loc);
	}
}

