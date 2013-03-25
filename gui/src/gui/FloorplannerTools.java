package gui;

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
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import floorplan.MasterMap;

public class FloorplannerTools extends JTabbedPane
{
	// Reference to the main floorplanner GUI
	private Floorplanner gui;
	
	// References to other things
	private FloorplannerEditTab edit_tab;
	
	/**
	 * Create a tool tab with the correct tab location
	 */
	FloorplannerTools(Floorplanner gui)
	{
		super(JTabbedPane.TOP);
		this.gui = gui;
		createEditTab();
	}
	
	private void createEditTab()
	{
		edit_tab = new FloorplannerEditTab(gui);
		addTab("Edit", null, edit_tab, null);
	}
	
	/**
	 * Updates the current masters map, should be called whenever the main master map changes
	 * as this function will contain bookkeeping calls to all other GUI elements 
	 */
	public void updateMasters(MasterMap new_masters)
	{
		edit_tab.updateMasters(new_masters);
	}
	
	public FloorplannerEditTab getEditTab() { return edit_tab; }

}
