package floorplan;

import java.io.File;
import java.util.Scanner;
import java.util.regex.Matcher;
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
						parseMasters(new File(c.file_name), masters);
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
						cur_master.addMasterInst(new Master(c.w, c.h), c.name, c.x, c.y);
					}
					// Non-atomic instantiation
					else
					{
						InstanceCommand c = new InstanceCommand(cmd);
						if (!masters.hasMaster(c.master_name))
							throw new Exception("Undefined floorplan master: " + c.master_name);
						cur_master.addMasterInst(masters.getMaster(c.master_name), c.name, c.x, c.y);						
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

}

/**
 * Various commands
 */
class IncludeCommand
{
	String file_name;
	IncludeCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 2)
			this.file_name = cmd[1];
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
	
}

class EndFloorplanCommand
{
	EndFloorplanCommand(String[] cmd) throws Exception
	{
		if (cmd.length != 1)
			throw new Exception("Syntax error: endfloorplan command does not expect arguments!");
	}
}

class AtomicCommand
{
	String name;
	double x, y, w, h;
	AtomicCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 6)
		{
			this.name = cmd[1];
			this.w = Double.parseDouble(cmd[2]);
			this.h = Double.parseDouble(cmd[3]);
			this.x = Double.parseDouble(cmd[4]);
			this.y = Double.parseDouble(cmd[5]);
		}
		else
			throw new Exception("Syntax error: atomic command expects 5 arguments!");
	}
}

class InstanceCommand
{
	String master_name;
	String name;
	double x, y;
	InstanceCommand(String[] cmd) throws Exception
	{
		if (cmd.length == 4)
		{
			this.master_name = cmd[0];
			this.name = cmd[1];
			this.x = Double.parseDouble(cmd[2]);
			this.y = Double.parseDouble(cmd[3]);
		}
		else
			throw new Exception("Syntax error: instance instantiations requires 3 arguments!");
	}
}