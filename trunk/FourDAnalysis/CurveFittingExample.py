import math, numpy, scipy.optimize

# ------------------------------------------------------------
# Define Curve to Fit
def gaussian(x, A, mean, sigma):
    global math, numpy
    gauss = A / math.sqrt(2.0*math.pi)/sigma * numpy.exp(-((x-mean)/sigma)**2/2)
    return(gauss)

# ------------------------------------------------------------
# Calculate error (the optimizer will try minimizing this function)
def residuals_with_error(param_fit, y, x):
    global gaussian
    A, mean, sigma, base = param_fit
    err = y - (gaussian(x, A, mean, sigma)+base)
    return(err)


# ------------------------------------------------------------
# Get the node ID of CurveAnalysisNode from local dictionary

execdict  = locals()
nodeID    = execdict['CurveAnalysisNodeID']
scene     = slicer.MRMLScene
curveNode = scene.GetNodeByID(nodeID)

sys.stderr.write('curveNode: %s\n' % ''.join( curveNode.GetName() ))

# ------------------------------------------------------------
# Get the node ID of CurveAnalysisNode from local dictionary
data = curveNode.GetSourceData()
nTuples = data.GetNumberOfTuples()
nComponents = data.GetNumberOfComponents()
sys.stderr.write(' number of tuples: %d\n' % nTuples)
sys.stderr.write(' number of components: %d\n' % nComponents)

x = []  # Time
y_meas = []  # Intensity value

for i in range(nTuples):
    x.append(data.GetValue(i*nComponents + 0))
    y_meas.append(data.GetValue(i*nComponents + 1))
    sys.stderr.write(' %f : ' % data.GetValue(i*nComponents + 0))
    sys.stderr.write(' %f\n' % data.GetValue(i*nComponents + 1))
                      

# Set initial parameter
# A, mean, sigma, base
param0 = [10.0, 0.0, 3, 2000.0]

# Perform optimization
param_output = scipy.optimize.leastsq(residuals_with_error, param0, args=(y_meas, x),full_output=True)
             
param_result = param_output[0] # fitted parameters
covar_result = param_output[1] # covariant matrix

# VTK's array to return the fitted curve
fittedCurve = curveNode.GetInterpolatedData();
fittedCurve.SetNumberOfComponents(2)
fittedCurve.SetNumberOfTuples(nTuples)

A, mean, sigma, base = param_result

sys.stderr.write(' A     : %f\n' % A )
sys.stderr.write(' mean  : %f\n' % mean )
sys.stderr.write(' sigma : %f\n' % sigma )
sys.stderr.write(' base  : %f\n' % base )


for i in range(nTuples):
    y = gaussian(x[i], A, mean, sigma) + base
    fittedCurve.SetTuple2(i, x[i], y)
    sys.stderr.write(' %f : ' % x[i])
    sys.stderr.write(' %f\n' % y)

curveNode.SetInterpolatedData(fittedCurve)
