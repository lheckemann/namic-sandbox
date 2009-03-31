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
        self.InitialOptimParam  = [0.1, 0.2, 0.01] 
        #self.InitialOptimParam  = [0.015, 0.12, 0.12] 
        #self.InitialOptimParam  = [0.05, 0.2, 0.2] 

        self.InputCurveNameList = ['AIF']

        self.AifTime = r_[0:33]
        self.AifData = r_[36.7619,
                          37.5714,
                          38.3333,
                          38.1905,
                          38.2857,
                          38.381 ,
                          38.8095,
                          33.0476,
                          51.1429,
                          240.286,
                          403.667,
                          428.857,
                          408.905,
                          383.476,
                          335.333,
                          288.667,
                          269.952,
                          262.81 ,
                          276.333,
                          291.095,
                          288.571,
                          286    ,
                          274.381,
                          260.81 ,
                          272    ,
                          269.19 ,
                          278.238,
                          272.905,
                          271.952,
                          266.095,
                          260.952,
                          263.714,
                          250.476]
        
        self.tck = splrep(self.AifTime, self.AifData, s=0)
        #self.dt = 0.001
        #self.AifTime2 = r_[0:30:self.dt]
        #self.AifTable = splev(self.AifTime2, self.tck, der=0)
        #print 'done.'
        
    # ------------------------------
    # Generate arteral input function from given data
    def SetInputData(self, name, curve):
        if name == 'AIF':
            self.AifTime = curve[0]
            self.AifData = curve[1]
            self.tck = splrep(self.AifTime, self.AifData, s=0)
        
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





