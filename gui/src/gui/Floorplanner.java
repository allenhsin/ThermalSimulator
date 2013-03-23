package gui;

import java.awt.Dimension;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import javax.swing.JLabel;
import display.FloorplanRender;
import floorplan.Floorplan;
import floorplan.FloorplanInst;

import javax.swing.border.BevelBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import java.awt.Color;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import javax.swing.JScrollPane;
import javax.swing.JList;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JScrollBar;
import javax.swing.ListSelectionModel;

import temperature.TemperatureTrace;

public class Floorplanner extends JFrame implements ListSelectionListener
{	
	Dimension render_size;
	Floorplan floorplan;
	FloorplanRender render;
	ToolTab tabbed_pane;
	
	private JTable objects_table;
	private JScrollPane objects_scroller;	

	public Floorplanner (String title, Dimension render_size, Floorplan floorplan) 
	{
		super (title);	
		
		this.render_size = render_size;
		this.floorplan = floorplan;
		
		initialize();
		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		this.pack();
		setResizable(true);
		setVisible (true);
	}

	public void redraw()
	{
		this.repaint();
	}
	
	private void initialize()
	{
		setLayout(new BorderLayout());

		tabbed_pane = new ToolTab(JTabbedPane.TOP);
		tabbed_pane.setPreferredSize(new Dimension(370, 550));
		getContentPane().add(tabbed_pane, BorderLayout.EAST);
//		getContentPane().add(tabbed_pane, BorderLayout.EAST);
		
		objects_table = new JTable(floorplan);
		objects_table.getSelectionModel().addListSelectionListener(this);
		tabbed_pane.addTab("Objects", null, objects_table, null);
		objects_scroller = new JScrollPane(objects_table);
		tabbed_pane.insertTab("Objects", null, objects_scroller, null, 0);
		
		render = new FloorplanRender (render_size);
		render.setFloorplan(floorplan);
		
		JPanel render_panel = new JPanel();
		render_panel.add(render);
		render_panel.setBorder(new EmptyBorder(5, 5, 5, 5));
		
		getContentPane().add(render_panel, BorderLayout.CENTER);
	}
	
	
	public void valueChanged(ListSelectionEvent e)
	{		
		ListSelectionModel lsm = (ListSelectionModel) e.getSource();
		List<FloorplanInst> highlights = render.getHighlights();
		highlights.clear();
		
		if (!lsm.isSelectionEmpty())
		{
			for (int i = lsm.getMinSelectionIndex(); i <= lsm.getMaxSelectionIndex(); ++i)
				if(lsm.isSelectedIndex(i)) highlights.add(floorplan.getFloorplanInsts().get(i));
		}
		render.repaint();
	}
}

class ToolTab extends JTabbedPane
{

	ToolTab (int tab_loc)
	{
		super(tab_loc);
	}
}