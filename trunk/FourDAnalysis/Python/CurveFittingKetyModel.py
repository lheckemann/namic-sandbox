# ----------------------------------------------------------------------
#
#  Python Package for Curve Fitting in 3D Slicer 4D Analysis Module
#
#   Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH)
#   All Rights Reserved.
#
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
#
#   Author: Junichi Tokuda (tokuda@bwh.harvard.edu)
#
#   For more detail, please refer:
#      http://wiki.na-mic.org/Wiki/index.php/Slicer3:FourDAnalysis
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# NOTE:
#   This python script requires SciPy package, which doesn't come with
#   Slicer3 package in default. Build 3D Slicer with USE_SCIPY option
#   (can be configured in slicer_variables.tcl) before run this script
#   from 3D Slicer.
# ----------------------------------------------------------------------

from FourDAnalysis import CurveAnalysisBase

import scipy, numpy
import sys

from scipy.integrate import quad
from scipy.integrate import fixed_quad
from scipy.integrate import quadrature
from scipy.interpolate import interp1d, splrep, splev
from numpy import r_

# ----------------------------------------------------------------------
# Kety Model
# ----------------------------------------------------------------------

class CurveFittingKetyModel(CurveAnalysisBase):

    # ------------------------------
    # Constructor -- Set initial parameters
    def __init__(self):
        self.OptimParamNameList = ['Ktrans', 'vp', 've']
        #self.InitialOptimParam  = [0.2, 1.0, 1.0] 
        self.InitialOptimParam  = [0.015, 0.12, 0.12] 
        #self.InitialOptimParam  = [0.05, 0.2, 0.2] 

        self.AifTime = r_[0:30]
        self.AifData = r_[40.1429,
                          43.1039,
                          41.7273,
                          474.117,
                          743.247,
                          819.422,
                          785.175,
                          776.857,
                          593.701,
                          481.416,
                          312.636,
                          277.273,
                          339.195,
                          423.636,
                          486.792,
                          538.506,
                          552.182,
                          549.955,
                          537.052,
                          508.961,
                          483.825,
                          493.701,
                          505.247,
                          460.688,
                          496.831,
                          513.864,
                          464.006,
                          470.487,
                          479.468,
                          465.305]

        self.tck = splrep(self.AifTime, self.AifData, s=0)
        #self.dt = 0.001
        #self.AifTime2 = r_[0:30:self.dt]
        #self.AifTable = splev(self.AifTime2, self.tck, der=0)
        #print 'done.'
        
    # ------------------------------
    # Arteral input function (AIF)
    def Aif(self, x):
        y = splev(x, self.tck, der=0)
        #y = self.AifTable[int(x/self.dt)]
        return y

    # ------------------------------
    # Definition of the function
    def Function(self, x, param):
        Ktrans, vp, ve = param
        lst = range(len(x))
        y = scipy.zeros(len(x))
        for i in lst:
            xx = x[i]
            s = quadrature(lambda t: self.Aif(t) * scipy.exp(-Ktrans*(xx-t)/ve), 0.0, xx, tol=1.0e-00, vec_func=False)
            #s = quadrature(lambda t: splev(t,self.tck,der=0) * scipy.exp(-Ktrans*(xx-t)/ve), 0.0, xx, vec_func=False)
            y[i] = vp * self.Aif(xx) + Ktrans  * s[0]
        return y

    # ------------------------------
    # Calculate the output parameters (called by GetOutputParam())
    def CalcOutputParam(self, param):
        Ktrans, vp, ve = param

        dict = {}
        dict['Ktrans'] = Ktrans
        dict['vp']     = vp
        dict['ve']     = ve
        
        sys.stderr.write('Ktrans     : %f\n' % Ktrans )
        sys.stderr.write('vp         : %f\n' % vp )
        sys.stderr.write('ve         : %f\n' % ve )

        #dict['alpha'] = alpha
        #dict['beta']  = beta
        #dict['Ta']    = Ta
        #dict['S0']    = S0

        return dict





