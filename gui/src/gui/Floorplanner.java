package gui;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import javax.swing.JLabel;
import display.FloorplanRender;
import display.RenderPanel;
import floorplan.Floorplan;
import floorplan.Master;
import floorplan.MasterInst;

import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreeSelectionModel;

import java.awt.Color;
import java.io.File;
import java.util.List;

import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JScrollBar;
import javax.swing.JTree;
import javax.swing.ListSelectionModel;

import temperature.TemperatureTrace;

public class Floorplanner extends JFrame implements ListSelectionListener, TreeSelectionListener
{	
	// The current top-level master instance
	private MasterInst top_inst;

	RenderPanel render_panel;
	ToolTab tabbed_pane;
	FloorplannerMenu menu_bar;
	
	private JTable objects_table;
	private JTree hier_tree;
	
	private JScrollPane objects_scroller, hier_scroller;	
	
	public Floorplanner (String title, Dimension render_size) 
	{
		super (title);	
		
		initialize(render_size);		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		this.pack();
		setResizable(true);
		setVisible (true);
	}
	
	public void createNewFloorplan()
	{
		top_inst = new MasterInst(new Master("Default"), "Top", 0.0, 0.0);
		hier_tree.setModel(new DefaultTreeModel(top_inst));
		
		Floorplan floorplan = new Floorplan(top_inst, null);
		objects_table.setModel(floorplan);
		render_panel.setFloorplan(floorplan);
	}
	
	public void openFloorplanFile(File fplan_file)
	{
		try
		{

			MasterInst top_inst = Master.parseMasters(fplan_file).getTop();

			hier_tree.setModel(new DefaultTreeModel(top_inst));

			Floorplan floorplan = new Floorplan(top_inst, null);
			objects_table.setModel(floorplan);
			render_panel.setFloorplan(floorplan);

		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(this, e.getMessage(), "Open", JOptionPane.WARNING_MESSAGE);
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
	
	public void redraw()
	{
		this.repaint();
	}
	
	private void initialize(Dimension render_size)
	{
		setLayout(new BorderLayout());
		menu_bar = new FloorplannerMenu(this);
		setJMenuBar(menu_bar);

		tabbed_pane = new ToolTab(JTabbedPane.TOP);
		tabbed_pane.setPreferredSize(new Dimension(450, 550));
		getContentPane().add(tabbed_pane, BorderLayout.WEST);
//		getContentPane().add(tabbed_pane, BorderLayout.EAST);
		
		hier_tree = new JTree();
		hier_tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);		
		hier_tree.getSelectionModel().addTreeSelectionListener(this);
		tabbed_pane.addTab("Hierarchy", null, hier_tree, null);
		hier_scroller = new JScrollPane(hier_tree);
		tabbed_pane.addTab("Hierarchy", null, hier_scroller, null);
		
		objects_table = new JTable();
		objects_table.setAutoCreateRowSorter(true);
		objects_table.getSelectionModel().addListSelectionListener(this);
		tabbed_pane.addTab("Objects", null, objects_table, null);
		objects_scroller = new JScrollPane(objects_table);
		tabbed_pane.addTab("Objects", null, objects_scroller, null);
	
		render_panel = new RenderPanel(render_size);
		getContentPane().add(render_panel, BorderLayout.CENTER);
	}
	
	
	public void valueChanged(ListSelectionEvent e)
	{		
		ListSelectionModel lsm = (ListSelectionModel) e.getSource();
		List<Floorplan> highlights = render_panel.getRender().getHighlights();
		highlights.clear();
		
		if (!lsm.isSelectionEmpty())
		{
			for (int i = lsm.getMinSelectionIndex(); i <= lsm.getMaxSelectionIndex(); ++i)
			{
				if(lsm.isSelectedIndex(i))
				{
					highlights.add(render_panel.getRender().getFloorplan().getChildrenMap().get(objects_table.getValueAt(i, 0)));
				}
			}
		}
		render_panel.repaint();
	}

	public void valueChanged(TreeSelectionEvent e) 
	{
		Floorplan f = new Floorplan((MasterInst) hier_tree.getLastSelectedPathComponent(), null);		
		render_panel.setFloorplan(f);		
		objects_table.setModel(f);
		render_panel.repaint();
	}
}

class ToolTab extends JTabbedPane
{
	ToolTab (int tab_loc)
	{
		super(tab_loc);
	}
}