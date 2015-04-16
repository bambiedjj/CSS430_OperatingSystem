/*
 *	Shell.java
 *
 *	This class functions as a Java thread that will be invoked from ThreadOS Loader 
 *	and behave as a shell command interpreter.
 *	
 *	By Jingjing Dong on 04/15/2015
 *	CSS_430_P1_part2
 */

import java.util.*;

public class Shell extends Thread {

	private int count;
	private String cmdLine;
	private String commands[];

	// Shell constructor
	public Shell () {
		count = 0;
	}

	// This method reads from command and runs each command from a line of command 
	// which can be seperated by either "&" or ";".
	// It's terminated when reading in "exit" command.
	public void run() {
		while (true) {
			printPrompt();
			parseCmd();
			// exit the current shell if receives "exit"
			if (commands[0].trim().equals("exit")) {
				SysLib.exit();
			 	return;
			}
			if (commands.length > 0 && !commands[0].equals("")) {
				for (int index = 0; index < commands.length; index++) {
					String concurrent[] = commands[index].split("&");	// '&' means a concurrent execution
					for (String cmd: concurrent) {
						cmd = cmd.trim();
						String execArgs[] = SysLib.stringToArgs(cmd);
                        SysLib.cout(execArgs[0]+"\n");
                        if ( SysLib.exec(execArgs) == -1 ) {
				    		SysLib.cerr( execArgs[0] + " failed in loading\n" );
				    		break;
						}
					}
                    SysLib.join();	// join sequential excutions 
                }
			}

		}
	}

	// This method read in command line and split by ";" into an array of String.
	private void parseCmd() {
		// read a line of commands
		StringBuffer buffer = new StringBuffer();
		SysLib.cin(buffer);
		cmdLine = buffer.toString().trim();
		// parse and splt line of commands
		commands = cmdLine.split(";");	// ';' means a sequential execution
	}

	// This method print prompt to entering shell commands as well as the 
	// current number of shell.
	public void printPrompt() {
		SysLib.cout("shell[");
		SysLib.cout(count+"");
		SysLib.cout("]% ");
		count++;
	}
}