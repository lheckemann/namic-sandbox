#! /Users/junichi/igtdev/slicer/Slicer3-lib/python-build/bin/python2.5

import sys
import scipy, scipy.optimize, scipy.io, numpy


if len(sys.argv) < 4:
    sys.exit()

ModuleName    = sys.argv[1]
InputCsvFile  = sys.argv[2]
OutputCsvFile = sys.argv[3]

module = __import__(ModuleName)

#from CurveFittingGammaVariate import  CurveFittingGammaVariate 

fitting = module.CurveFittingGammaVariate()
data    = scipy.io.read_array(InputCsvFile, separator=',')
fitting.SetSourceCurve(data)
fitting.Execute()

x1          = numpy.arange(0.0, 30.0)
y           = fitting.GetFitCurve(x1)

fittedCurve = scipy.zeros([x1.shape[0], 2])
fittedCurve[:, 0] = x1
fittedCurve[:, 1] = y
scipy.io.write_array(OutputCsvFile, fittedCurve, separator=',')

result = fitting.GetOutputParam()
for i, v, in result.iteritems():
    sys.stderr.write(' %5s     : %f\n'  % (i, v) )



