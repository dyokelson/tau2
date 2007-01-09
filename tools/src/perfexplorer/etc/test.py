from server import PerfExplorerServer
from common import TransformationType
from common import AnalysisType

print "--------------- JPython test script start ------------"

x = 2 + 5
print x

peserver = PerfExplorerServer.getInstance()
peserver.doSomething()

# let's do something interesting...

peserver.setApplication("Miranda")
peserver.setExperiment("BlueGeneL")
peserver.setTrial("8K.old")
peserver.setMetric("Time")
peserver.setDimensionReduction(TransformationType.OVER_X_PERCENT, "2")
peserver.setAnalysisType(AnalysisType.K_MEANS)
peserver.requestAnalysis()

print "---------------- JPython test script end -------------"
