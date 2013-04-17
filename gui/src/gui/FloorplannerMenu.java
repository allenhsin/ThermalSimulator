package gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.Iterator;
import java.util.Vector;

import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.filechooser.FileNameExtensionFilter;

import floorplan.FillerTree;
import floorplan.GridPoint;
import floorplan.Master;
import floorplan.MasterInst;

/**
 * A class containing the menubar of the floorplanner GUI 
 *
 */
class FloorplannerMenu extends JMenuBar
{
	
	private Floorplanner gui;
	
	/**
	 * Constructor for the menu bar, contains a pointer to the main menu
	 * @param gui, floorplanner gui that the menu bar is a part of
	 */
	FloorplannerMenu(Floorplanner gui)
	{
		super();
		this.gui = gui;
		
		createMenus();
	}
	
	/**
	 * Helper function that creates all the menus
	 */
	private void createMenus()
	{
		// Create Menus
		JMenu file_menu = new JMenu("File");
		JMenu tools_menu = new JMenu("Tools");		
		
		FileMenuListener file_menu_listener = new FileMenuListener(gui);
		ToolsMenuListener tools_menu_listener = new ToolsMenuListener(gui);
		
		add(file_menu);
		add(tools_menu);
		
		// Create items in file menu
		addMenuItem(file_menu, file_menu_listener, new JMenuItem("New Floorplan"),
				KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
		addMenuItem(file_menu, file_menu_listener, new JMenuItem("Open Floorplan..."),
				KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
		addMenuItem(file_menu, file_menu_listener, new JMenuItem("Save Floorplan As..."),		
		KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
		file_menu.addSeparator();
		addMenuItem(file_menu, file_menu_listener, new JMenuItem("Open Temperature Trace..."),
				KeyStroke.getKeyStroke(KeyEvent.VK_T, ActionEvent.CTRL_MASK));
		file_menu.addSeparator();
		addMenuItem(file_menu, file_menu_listener, new JMenuItem("Exit"),
				KeyStroke.getKeyStroke(KeyEvent.VK_F4, ActionEvent.ALT_MASK));
				
		addMenuItem(tools_menu, tools_menu_listener, new JMenuItem("Generate Fill"),
				KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
	}

	/**
	 * Helper for adding a menu item and automatically adding the menu listener with a
	 * keystroke shortcut
	 */
	private void addMenuItem(JMenu menu, ActionListener listener, JMenuItem item, KeyStroke key)
	{
		item.setAccelerator(key);
		addMenuItem(menu, listener, item);
	}

	/**
	 * Helper for adding a menu item and automatically adding the menu listener
	 */
	private void addMenuItem(JMenu menu, ActionListener listener, JMenuItem item)
	{
		item.addActionListener(listener);
		menu.add(item);
	}
}

/**
 * Helper class for handling all the listener events from the file menu
 */
class FileMenuListener extends EventsHelper<Floorplanner> implements ActionListener
{
	// File chooser used
	JFileChooser fc;
	
	/**
	 * Floorplanner listener
	 */
	FileMenuListener(Floorplanner gui)
	{
		super(gui);
		this.fc = new JFileChooser();
	}
	/**
	 * Listen for menu actions
	 */
	public void actionPerformed(ActionEvent e)
	{
		String cmd = e.getActionCommand();
		if (cmd.equals("New Floorplan"))
		{
			owner.createNewFloorplan();
		}
		else if (cmd.equals("Open Floorplan..."))
		{
			fc.setFileFilter(new FileNameExtensionFilter("Floorplan file (*.flp)", "flp"));
			fc.showOpenDialog(owner);
			if (fc.getSelectedFile() != null)
				owner.openFloorplanFile(fc.getSelectedFile());
		}
		else if (cmd.equals("Save Floorplan As..."))
		{
			fc.setFileFilter(new FileNameExtensionFilter("Floorplan file (*.flp)", "flp"));
			fc.showSaveDialog(owner);			
			if (fc.getSelectedFile() != null)
				owner.saveFloorplanFile(fc.getSelectedFile());
		}
		else if (cmd.equals("Open Temperature Trace..."))
		{
			fc.setFileFilter(new FileNameExtensionFilter("Temperature trace (*.out)", "out"));
			fc.showOpenDialog(owner);			
			if (fc.getSelectedFile() != null)
				owner.openTemperatureTrace(fc.getSelectedFile());
		}
		else if (cmd.equals("Exit"))
		{
			System.exit(0);
		}
		else throw new Error("Internal Error: Menu Operation '" + cmd + "' is not supported!");
		
	}	
}

/**
 * Helper class for handling all the listener events from the tools menu
 */
class ToolsMenuListener extends EventsHelper<Floorplanner> implements ActionListener
{
	/**
	 * Floorplanner listener
	 */
	ToolsMenuListener(Floorplanner gui)
	{
		super(gui);
	}

	/**
	 * Listen for menu actions
	 */
	public void actionPerformed(ActionEvent e)
	{
		String cmd = e.getActionCommand();
		if (cmd.equals("Generate Fill"))
		{
			Master m = owner.getCurMaster();
			Master fill_master = Master.createFillerMaster(m);
			try
			{
				owner.getMasters().addMaster(fill_master);
				m.addMasterInst(fill_master, "Filler", GridPoint.ZERO, GridPoint.ZERO);			
				owner.updateMasters();
			}
			catch (Exception ex)
			{
				JOptionPane.showMessageDialog(owner, "Fill generation failed: " + ex.getMessage(),
						"Filler generation", JOptionPane.WARNING_MESSAGE);
			}						
		}
		else throw new Error("Internal Error: Menu Operation '" + cmd + "' is not supported!");
		
	}	
}

