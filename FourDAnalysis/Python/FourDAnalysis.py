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

import re
import sys
import imp
import scipy.optimize 

# ----------------------------------------------------------------------
# Base class for curve fitting algorithm classes
# ----------------------------------------------------------------------

class CurveAnalysisBase(object):
    OptimParamNameList    = []
    InitialOptimParam     = []
    
    OptimParam        = []
    CovarianceMatrix  = []
    SourceCurve       = []

    #def __init__(self):
    #    ## OptimParamNameList and Initial Param should be set here
        
    def Function(self, x, param):
        return 0

    def ResidualError(self, param, y, x):
        err = y - (self.Function(x, param))
        return err
    
    def GetOptimParamNameList(self):
        return self.OptimParamNameList

    def GetOutputParamNameList(self):
        dict = self.CalcOutputParam(self.InitialOptimParam)
        list = []
        for key, value in dict.iteritems():
            list.append(key)
        return list

    def SetInitialOptimParam(self, param):
        self.InitialOptimParam = param

    def GetInitialOptimParam(self):
        return self.InitialOptimParam
    
    def GetOptimParam(self):
        return self.OptimParam

    def GetOutputParam(self):
        return self.CalcOutputParam(self.OptimParam)
    
    def SetSourceCurve(self, sourceCurve):
        self.SourceCurve = sourceCurve

    def Execute(self):

        x      = self.SourceCurve[:, 0]
        y_meas = self.SourceCurve[:, 1]

        param0 = self.InitialOptimParam
        param_output = scipy.optimize.leastsq(self.ResidualError, param0, args=(y_meas, x),full_output=False,ftol=1e-04,xtol=1.49012e-04)
        self.OptimParam       = param_output[0] # fitted parameters
        self.CovarianceMatrix = param_output[1] # covariant matrix
        
        return 1        ## shoud return 0 if optimization fails

    def GetFitCurve(self, x):
        return self.Function(x, self.OptimParam)



# ----------------------------------------------------------------------
# Class to execute curve fitting
# ----------------------------------------------------------------------

class CurveAnalysisExecuter(object):

    ModuleName = ''
    DebugMode = 0

    # ------------------------------
    # Constructor
    def __init__(self, modulePath):
        # ------------------------------
        # Extract directory path, file name and module name
        
        direxp = re.compile('([^\/]+)$')
        extexp = re.compile('(\.py)$')
        
        directory = direxp.sub('', modulePath)
        fileNameMatch = direxp.search(modulePath)
        fileName = fileNameMatch.groups()[0]
        self.ModuleName = extexp.sub('', fileName)

        sys.stderr.write('Directory     : %s\n' % directory )
        sys.stderr.write('File name     : %s\n' % fileName )
        sys.stderr.write('File name     : %s\n' % fileName )

        # ------------------------------
        # Add the search path if it hasn't been registered.
        try:
            id = sys.path.search(directory)
        except:
            sys.path.append(directory)

        # ------------------------------
        # Load / reload the module.

        fp, pathname, description = imp.find_module(self.ModuleName)
        try:
            self.Module = imp.load_module(self.ModuleName, fp, pathname, description)
        finally:
            if fp:
                fp.close()

    # ------------------------------
    # Get Output Parameter Name List
    def GetOutputParameterNames(self):
        exec('fitting = self.Module.' + self.ModuleName + '()')
        list =  fitting.GetOutputParamNameList()
        for i in list:
            sys.stderr.write('name     : %s\n' % i )
        return list


    # ------------------------------
    # Call curve fitting class
    def Execute(self, inputCurve, outputCurve):
        exec('fitting = self.Module.' + self.ModuleName + '()')
        fitting.SetSourceCurve(inputCurve)
        fitting.Execute()
        x = outputCurve[:, 0]
        y = fitting.GetFitCurve(x)
        
        outputCurve[:, 1] = y
        
        result = fitting.GetOutputParam()

        return result
