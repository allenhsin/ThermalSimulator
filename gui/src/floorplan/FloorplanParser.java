package floorplan;

import java.io.File;
import java.io.FileWriter;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Scanner;
import java.util.regex.Pattern;

/**
 * Class that parses a floorplan file (or several floorplan files)
 */
public class FloorplanParser
{
	// Constants
	public static final String COMMENT_DELIMITER = "#";
	public static final Pattern NONEMPTY_PATTERN = Pattern.compile("\\S");
	public static final Pattern COMMENT_PATTERN = Pattern.compile(COMMENT_DELIMITER);
	public static final Pattern COMMAND_PATTERN = Pattern.compile("\\s+");
	
	// Commands
	public static final String CMD_INCLUDE = "include";
	public static final String CMD_FLOORPLAN = "floorplan";
	public static final String CMD_ENDFLOORPLAN = "endfloorplan";
	public static final String CMD_ATOMIC = "atomic";
	public static final String CMD_FILLER = "filler";
	
	// Private constructor
	private FloorplanParser() {}	
	
	/**
	 * Parse masters from a file
	 * @param file
	 * @return MasterMap containing all the read masters
	 * @throws Exception
	 */
	public static MasterMap parseMasters(File file) throws Exception
	{
		// Create HashMap with a mapping to all known masters
		MasterMap masters = new MasterMap();
		// Populate the masters map
		parseMasters(file, masters);
		return masters;
	}
	
	/**
	 * Parse masters helper function
	 */
	private static void parseMasters(File file, MasterMap masters) throws Exception
	{
		// Open the file with a scanner
		Scanner s = new Scanner(file);
		
		// Loop state variables
		int line_num = 0;
		Master cur_master = null;
		
		// Main parsing loop
		while (s.hasNextLine())
		{			
			// Get the line and strip away preceding/postceding whitespace and comments
			String line = stripComments(s.nextLine()).trim();
			++line_num;
			
			// If the line is not empty, read it
			if (NONEMPTY_PATTERN.matcher(line).find())
			{
				String[] cmd = parseCommand(line);
				try
				{
					// File inclusion
					if (cmd[0].equals(CMD_INCLUDE))
					{
						IncludeCommand c = new IncludeCommand(cmd);
						// Setup path to the included file, relative to the path of the current file
						File include_file = new File(file.getParentFile().getPath() + File.separator + c.file_location);
						parseMasters(include_file, masters);
					}
					// Floorplan definition
					else if (cmd[0].equals(CMD_FLOORPLAN))
					{
						FloorplanCommand c = new FloorplanCommand(cmd);
						cur_master = new Master(c.name);
						if (masters.hasMaster(c.name))
							throw new Exception("Duplicate floorplan definition: " + c.name);
						masters.addMaster(cur_master);
					}				
					// End floorplan definition
					else if (cmd[0].equals(CMD_ENDFLOORPLAN))
					{
						new EndFloorplanCommand(cmd);
						cur_master = null;
					}
					// Atomic instantiation
					else if (cmd[0].equals(CMD_ATOMIC))
					{
						AtomicCommand c = new AtomicCommand(cmd);
						if (cur_master == null)
							throw new Exception("Instances may only be added inside of a floorplan.");
						if (!cur_master.addMasterInst(new Master(c.w, c.h, false), c.name, c.x, c.y))
							throw new Exception("Instance '" + c.name + "' has a duplicate instance name");
					}
					// Atomic filler instantiation
					else if (cmd[0].equals(CMD_FILLER))
					{
						FillerCommand c = new FillerCommand(cmd);
						if (cur_master == null)
							throw new Exception("Instances may only be added inside of a floorplan.");
						if (!cur_master.addMasterInst(new Master(c.w, c.h, true), c.name, c.x, c.y))
							throw new Exception("Instance '" + c.name + "' has a duplicate instance name");						
					}
					// Non-atomic instantiation
					else
					{
						InstanceCommand c = new InstanceCommand(cmd);
						if (!masters.hasMaster(c.master_name))
							throw new Exception("Undefined floorplan master: " + c.master_name);
						if(!cur_master.addMasterInst(masters.getMaster(c.master_name), c.name, c.x, c.y))						
							throw new Exception("Instance '" + c.name + "' has a duplicate instance name");
					}
				}
				catch (Exception e)
				{
					s.close();
					throw new Exception(file.getName() + " line " + line_num + ": " + e.getMessage());
				}
			}
		}		
		s.close();
	}
	
	/**
	 * Helper for stripping away the comments in a line
	 * @param line to strip comments
	 * @return Line with no comments
	 */
	private static String stripComments(String line)
	{
		return COMMENT_PATTERN.split(line)[0];
	}
	
	/**
	 *  Helper for parsing the command from a line
	 * @param line to parse the command
	 * @return String[] containing the command and arguments 
	 */
	private static String[] parseCommand(String line)
	{
		return COMMAND_PATTERN.split(line);
	}

	
	/**
	 * Writes masters to a file. It identifies the dependencies of the masters
	 * and only writes a master when its dependencies have already been taken care
	 * of
	 */	
	public static void writeMasters(File file, MasterMap masters) throws Exception
	{
		FileWriter writer = new FileWriter(file);
		Hashtable<String, Master> masters_to_write = new Hashtable<String, Master>();
		masters_to_write.putAll(masters.getMasters());
		
		try
		{
			// While there are masters to write
			while(masters_to_write.size() > 0)
			{
				
				// Initialize variables/iterator
				Master master_to_write = null;
				Iterator<Master> master_it = masters_to_write.values().iterator();
				// Keep searching until it runs out and while it hasn't found a master to write yet
				while((master_to_write == null) && master_it.hasNext())
				{
					// Initialize current master to a dependent-less state
					Master cur_master = master_it.next();
					boolean has_dependents = false;
					
					// Keep searching for dependents until either it finds one or until there are no
					// subinstances left
					Iterator<MasterInst> inst_it = cur_master.getInstances().iterator();
					while (!has_dependents && inst_it.hasNext())
					{
						MasterInst cur_inst = inst_it.next();
						// If the instance is not atomic and it is still in the list of masters to
						// write, then the master has dependents
						if (!cur_inst.isAtomic() && masters_to_write.containsKey(cur_inst.m.getName()))
							has_dependents = true;
					}
					
					// If the current master has no unwritten dependencies, then set it as the master to write
					if (!has_dependents)
						master_to_write = cur_master;
				}
				
				// If we have exhausted all the possibilities
				if (master_to_write == null)
				{
					throw new Exception("All remaining masters have unresolved dependencies");
				}
				// If we have found a master with no unresolved dependents
				else
				{
					// Write the master to the file
					writeMaster(writer, master_to_write);
					// Remove from masters to write
					masters_to_write.remove(master_to_write.getName());
				}
				
			}
			
		}
		catch (Exception e)
		{
			writer.close();
			throw new Exception(e.getMessage());
		}
		
		writer.close();
	}
	
	/**
	 * Writes a master to a file
	 */
	private static void writeMaster(FileWriter writer, Master master) throws Exception
	{
		writer.write(String.format("# Floorplan %s, %d instances\n",
				master.getName(), master.getInstances().size()));
		writer.write(new FloorplanCommand(master).getCommand("") + "\n");
		
		Iterator<MasterInst> it = master.getInstances().iterator();
		while(it.hasNext())
		{
			MasterInst inst = it.next();
			if(inst.isAtomic())
			{
				if (inst.isFiller())
					writer.write(new FillerCommand(inst).getCommand("    ") + "\n");
				else
					writer.write(new AtomicCommand(inst).getCommand("    ") + "\n");
			}
			else
				writer.write(new InstanceCommand(inst).getCommand("    ") + "\n");
		}
		
		writer.write(new EndFloorplanCommand(master).getCommand("") + "\n");
		writer.write("\n");
	}
	
	/**
	 * 
	 */
}

/**
 * Various commands
 */
class IncludeCommand
{
	String file_location;
	IncludeCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 2)
			this.file_location = cmd[1];
		else
			throw new Exception("Syntax error: floorplan command expects 1 argument!");
	}
}

class FloorplanCommand
{
	String name;
	FloorplanCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 2)
			this.name = cmd[1];
		else
			throw new Exception("Syntax error: floorplan command expects 1 argument!");
	}
	
	FloorplanCommand(Master m) throws Exception
	{
		if (!m.isAtomic())
			this.name = m.getName();
		else
			throw new Exception("Master was atomic!");
	}
	
	String getCommand(String indent)
	{
		return String.format("%sfloorplan %s", indent, name);
	}
	
}

class EndFloorplanCommand
{
	EndFloorplanCommand(String[] cmd) throws Exception
	{
		if (cmd.length != 1)
			throw new Exception("Syntax error: endfloorplan command does not expect arguments!");
	}
	
	EndFloorplanCommand(Master m) throws Exception
	{
		if (m.isAtomic())
			throw new Exception("Master was atomic!");
	}
	
	String getCommand(String indent)
	{
		return String.format("%sendfloorplan", indent);
	}
}

class AtomicCommand
{
	String name;
	GridPoint x, y, w, h;
	AtomicCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 6)
		{
			this.name = cmd[1];
			this.w = GridPoint.parseGridPoint(cmd[2]);
			this.h = GridPoint.parseGridPoint(cmd[3]);
			this.x = GridPoint.parseGridPoint(cmd[4]);
			this.y = GridPoint.parseGridPoint(cmd[5]);
		}
		else if (cmd.length == 7)
		{
			this.name = cmd[1];
			this.w = GridPoint.parseGridPoint(cmd[2]);
			this.h = GridPoint.parseGridPoint(cmd[3]);
			this.x = GridPoint.parseGridPoint(cmd[4]);
			this.y = GridPoint.parseGridPoint(cmd[5]);
		}
		else
			throw new Exception("Syntax error: atomic command expects 5 arguments!");
	}
	
	AtomicCommand(MasterInst m) throws Exception
	{
		if (m.isAtomic())
		{
			this.name = m.n;
			this.x = m.x;
			this.y = m.y;
			this.w = m.m.getWidth();
			this.h = m.m.getHeight();
		}
		else
			throw new Exception("Master instance was not atomic!");
	}
	
	String getCommand(String indent)
	{
		return String.format("%satomic %s %s %s %s %s", indent, 
				name, w, h, x, y);
	}
}

class FillerCommand
{
	String name;
	GridPoint x, y, w, h;
	FillerCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 6)
		{
			this.name = cmd[1];
			this.w = GridPoint.parseGridPoint(cmd[2]);
			this.h = GridPoint.parseGridPoint(cmd[3]);
			this.x = GridPoint.parseGridPoint(cmd[4]);
			this.y = GridPoint.parseGridPoint(cmd[5]);
		}
		else if (cmd.length == 7)
		{
			this.name = cmd[1];
			this.w = GridPoint.parseGridPoint(cmd[2]);
			this.h = GridPoint.parseGridPoint(cmd[3]);
			this.x = GridPoint.parseGridPoint(cmd[4]);
			this.y = GridPoint.parseGridPoint(cmd[5]);
		}
		else
			throw new Exception("Syntax error: filler command expects 5 arguments!");
	}
	
	FillerCommand(MasterInst m) throws Exception
	{
		if (m.isAtomic() && m.isFiller())
		{
			this.name = m.n;
			this.x = m.x;
			this.y = m.y;
			this.w = m.m.getWidth();
			this.h = m.m.getHeight();
		}
		else
			throw new Exception("Master instance was not an atomic filler!");
	}
	
	String getCommand(String indent)
	{
		return String.format("%sfiller %s %s %s %s %s", indent, 
				name, w, h, x, y);
	}
}

class InstanceCommand
{
	String master_name;
	String name;
	GridPoint x, y;
	InstanceCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 4)
		{
			this.master_name = cmd[0];
			this.name = cmd[1];
			this.x = GridPoint.parseGridPoint(cmd[2]);
			this.y = GridPoint.parseGridPoint(cmd[3]);
		}
		else
			throw new Exception("Syntax error: instance instantiations requires 3 arguments!");
	}
	
	InstanceCommand(MasterInst m) throws Exception
	{
		if (!m.isAtomic())
		{
			this.master_name = m.m.getName();
			this.name = m.n;
			this.x = m.x;
			this.y = m.y;
		}
		else
			throw new Exception("Master instance was atomic!");
	}

	String getCommand(String indent)
	{
		return String.format("%s%s %s %s %s", indent, 
				master_name, name, x, y);
	}
}