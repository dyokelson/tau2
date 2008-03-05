package glue.psl;

import junit.framework.TestCase;
import glue.Utilities;
import edu.uoregon.tau.perfdmf.Trial;
import glue.TrialResult;

public class ExperimentTest extends TestCase {

	public final void testExperimentTrialResult() {
	    Utilities.setSession("PERI_DB_production");
	    Trial baseline = Utilities.getTrial("gtc", "jaguar", "64");
	    TrialResult baseResult = new TrialResult(baseline);

		Application app = new Application("test");
		Version version = new Version(app, "test");
		Experiment exp = new Experiment(version, baseResult);
		for (SourceFile file : version.getSourceFiles()) {
			System.out.println(file.getName());
		}
	}

}
