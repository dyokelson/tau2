package common;

import edu.uoregon.tau.perfdmf.Database;
import edu.uoregon.tau.perfdmf.database.ConnectionManager;
import edu.uoregon.tau.perfdmf.Database;
import edu.uoregon.tau.perfdmf.database.DB;
import edu.uoregon.tau.perfdmf.database.ParseConfig;

import jargs.gnu.CmdLineParser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;

import java.sql.ResultSet;
import java.sql.SQLException;

/**
 * This class is used as a main class for configuring PerfExplorer.
 *
<<<<<<< Configure.java
 * <P>CVS $Id: Configure.java,v 1.9 2008/03/05 00:25:53 khuck Exp $</P>
=======
 * <P>CVS $Id: Configure.java,v 1.9 2008/03/05 00:25:53 khuck Exp $</P>
>>>>>>> 1.7.8.4
 * @author  Kevin Huck
 * @version 0.1
 * @since   0.1
 */
public class Configure {

    
    private static String Greeting = "\nNow testing your database connection.\n";
    private static String Usage = "Usage: configure [{-h,--help}] [{-g,--configfile} filename] [{-t,--tauroot} path]";

    private String tau_root = "";
    private String db_dbname = "perfdmf";
    private String db_password = "";
    private ParseConfig parser;

    private String configFileName;
    
    private String perfExplorerSchema = null;

    /**
     * Public constructor.
     *
     * @param tauroot
     * @param arch
     */
    public Configure(String tauroot, String arch) {
        super();
        this.tau_root = tauroot;
    }

    /**
     * The main method for performing configuration.
     *
     * @param configFileNameIn
     */
    public void initialize(String configFileNameIn) {
        // Welcome the user to the program
        PerfExplorerOutput.println(Greeting);

        try {
            // Check to see if the configuration file exists
            configFileName = configFileNameIn;
            File configFile = new File(configFileName);
            if (configFile.exists()) {
                PerfExplorerOutput.println("Configuration file found...");
                parseConfigFile();
            } else {
                PerfExplorerOutput.println("Configuration file NOT found...");
                PerfExplorerOutput.println("a new configuration file will be created.");
                // If it doesn't exist, explain that the program looks for the 
                // configuration file in ${PerfDMF_Home}/data/perfdmf.cfg
                // Since it isn't there, create a new one.
            }
        } catch (IOException e) {
            // todo - get info from the exception
            PerfExplorerOutput.println("I/O Error occurred.");
            System.err.println(e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
    
    /**
     * Method for parsing the PerfDMF configuration file.
     *
     * @throws IOException
     * @throws FileNotFoundException
     */
    public void parseConfigFile() throws IOException, FileNotFoundException {
        PerfExplorerOutput.println("Parsing config file...");
        parser = new ParseConfig(configFileName);
        db_dbname = parser.getDBName();
        db_password = parser.getDBPasswd();
    }

    /**
     * Method for testing that the database tables exist.
     * If the tables don't exist, create them in the database.
     *
     */
    public void createDB() {
        ConnectionManager connector = null;
        DB db = null;
        try {
            if (db_password != null) {
                connector = new ConnectionManager(new Database(configFileName), db_password);
            } else {
                connector = new ConnectionManager(new Database(configFileName));
            }
            connector.connect();
            PerfExplorerOutput.println();
            db = connector.getDB();
        } catch (Exception e) {
            StringBuffer buf = new StringBuffer();
            buf.append("\nPlease make sure that your DBMS is configured ");
            buf.append("correctly, and the database ");
            buf.append(db_dbname + " has been created.");
            PerfExplorerOutput.println(buf.toString());
            System.exit(1);
        }

        try {
            StringBuffer query = new StringBuffer();
            query.append("SELECT * FROM ");
            query.append(db.getSchemaPrefix() + "analysis_settings");
            ResultSet resultSet = db.executeQuery(query.toString());
            resultSet.close();
        } catch (SQLException e) {
            // this is our method of determining that no 'application' table exists

            PerfExplorerOutput.print("Perfexplorer tables not found.");
            String input = "";

            if (perfExplorerSchema == null) {
	            PerfExplorerOutput.print("Would you like to upload the schema? [y/n]: ");
	            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
	
	            try {
	                input = reader.readLine();
	            } catch (java.io.IOException ioe) {
	                PerfExplorerOutput.println("I/O Error occurred.");
	                System.err.println(e.getMessage());
	                ioe.printStackTrace();
	                System.exit(-1);
	            }
            } else {
            	input = "y";
            }
            
            if (input.equals("y") || input.equals("Y")) {

                String filename="";
                
                if (perfExplorerSchema == null) {
	                if (db.getDBType().compareTo("oracle") == 0) {
	                    filename = tau_root + "/etc/dbschema.oracle";
	                } else if (db.getDBType().compareTo("derby") == 0) {
	                    filename = tau_root + "/etc/dbschema.derby";
	                } else if (db.getDBType().compareTo("mysql") == 0) {
	                    filename = tau_root + "/etc/dbschema.mysql";
	                } else if (db.getDBType().compareTo("postgresql") == 0) {
	                    filename = tau_root + "/etc/dbschema.postgresql";
	                } else if (db.getDBType().compareTo("db2") == 0) {
	                    filename = tau_root + "/etc/dbschema.db2";
	                } else {
	                    PerfExplorerOutput.println("Unknown database type: " + db.getDBType());
	                    System.exit(-1);
	                }
                } else {
                	filename = perfExplorerSchema;
                }
                
                
                PerfExplorerOutput.println("Uploading Schema: " + filename);
                if (connector.genParentSchema(filename) == 0) {
                    PerfExplorerOutput.println("Successfully uploaded schema\n");
                } else {
                    System.err.println("Error uploading schema\n");
                    System.exit(1);
                }
            }
        }

        try {
            if (db.checkSchema() != 0) {
                System.err.print("\nIncompatible schema found.  ");
                System.err.println("Please contact us at tau-team@cs.uoregon.edu");
                System.err.println("for a conversion script.");
                System.exit(1);
            }
        } catch (SQLException e) {
            System.err.println("\nError trying to confirm schema:");
            e.printStackTrace();
            System.exit(1);
        }

        connector.dbclose();

        PerfExplorerOutput.println("Database connection successful.");
        PerfExplorerOutput.println("Configuration complete.");
    }

    public static void main(String[] args) {

        CmdLineParser parser = new CmdLineParser();
        CmdLineParser.Option configfileOpt = parser.addStringOption('g', "configfile");
        CmdLineParser.Option homeOpt = parser.addStringOption('t', "tauroot");
        CmdLineParser.Option archOpt = parser.addStringOption('a', "arch");
        CmdLineParser.Option helpOpt = parser.addBooleanOption('h', "help");
        try {
            parser.parse(args);
        } catch (CmdLineParser.OptionException e) {
            System.err.println(e.getMessage());
            System.err.println(Usage);
            System.exit(-1);
        }

        String configFile = (String) parser.getOptionValue(configfileOpt);
        String tauroot = (String) parser.getOptionValue(homeOpt);
        String arch = (String) parser.getOptionValue(archOpt);
        Boolean help = (Boolean) parser.getOptionValue(helpOpt);

        if (help != null && help.booleanValue()) {
            System.err.println(Usage);
            System.exit(-1);
        }

        if (configFile == null)
            configFile = new String("");
        if (tauroot == null)
            tauroot = new String("");
        if (arch == null)
            arch = new String("");

        // Create a new Configure object, which will walk the user through
        // the process of creating/editing a configuration file.
        Configure config = new Configure(tauroot, arch);

        config.tau_root = tauroot;

        config.initialize(configFile);
        config.createDB();

    }

	public String getPerfExplorerSchema() {
		return perfExplorerSchema;
	}

	public void setPerfExplorerSchema(String perfexplorerSchema) {
		this.perfExplorerSchema = perfexplorerSchema;
	}

}
