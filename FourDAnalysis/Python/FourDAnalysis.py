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
import numpy

# ----------------------------------------------------------------------
# Base class for curve fitting algorithm classes
# ----------------------------------------------------------------------

class CurveAnalysisBase(object):

    OptimParamNameList    = []
    InitialOptimParam     = []
    InputCurveNameList    = []
    
    OptimParam        = []
    CovarianceMatrix  = []
    TargetCurve       = []

    #def __init__(self):
    #    ## OptimParamNameList and Initial Param should be set here
        
    def Function(self, x, param):
        return 0

    def ResidualError(self, param, y, x):
        err = y - (self.Function(x, param))
        return err

    def CalcOutputParamDict(self, param):
        return {}

    # ------------------------------
    # Convert signal intensity curve to concentration curve
    def SignalToConcent(self, signal):
        return signal

    # ------------------------------
    # Convert concentration curve to signal intensity curve
    # (used to generate a fit curve)
    def ConcentToSignal(self, concent):
        return concent

    def GetOptimParamNameList(self):
        return self.OptimParamNameList

    def GetInputCurveNameList(self):
        return self.InputCurveNameList

    def GetOutputParamNameList(self):
        dict = self.CalcOutputParamDict(self.InitialOptimParam)
        list = []
        for key, value in dict.iteritems():
            list.append(key)
        return list

    def SetInitialOptimParam(self, param):
        self.InitialOptimParam = param

    def GetInitialOptimParam(self):
        return self.InitialOptimParam

    def SetInputCurve(self, name, curve):
        return 0
    
    def GetOptimParam(self):
        return self.OptimParam

    def GetOutputParam(self):
        return self.CalcOutputParamDict(self.OptimParam)

    def SetTargetCurve(self, sourceCurve):
        self.TargetCurve = sourceCurve

    def Execute(self):
        x      = self.TargetCurve[:, 0]
        y_meas = self.SignalToConcent(self.TargetCurve[:, 1])

        param0 = self.InitialOptimParam

        param_output = scipy.optimize.leastsq(self.ResidualError, param0, args=(y_meas, x),full_output=False,ftol=1e-04,xtol=1.49012e-04)
        self.OptimParam       = param_output[0] # fitted parameters
        self.CovarianceMatrix = param_output[1] # covariant matrix
        
        return 1        ## should return 0 if optimization fails

    def GetFitCurve(self, x):
        return self.ConcentToSignal(self.Function(x, self.OptimParam))



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
    # Get Input Curve Name List
    def GetInputCurveNames(self):
        exec('fitting = self.Module.' + self.ModuleName + '()')
        list =  fitting.GetInputCurveNameList()
        return list

    # ------------------------------
    # Get Initial Optimization Parameter List
    def GetInitialOptimParams(self):
        exec('fitting = self.Module.' + self.ModuleName + '()')
        names  = fitting.GetOptimParamNameList()
        values = fitting.GetInitialOptimParam()
        n = len(names)
        params = {}
        for i in range(n):
            params[names[i]] = values[i]
        return params

    # ------------------------------
    # Get Output Parameter Name List
    def GetOutputParameterNames(self):
        exec('fitting = self.Module.' + self.ModuleName + '()')
        list =  fitting.GetOutputParamNameList()
        return list

    # ------------------------------
    # Call curve fitting class
    def Execute(self, inputCurvesDict, initialOptimParamDict, targetCurve, outputCurve):

        exec('fitting = self.Module.' + self.ModuleName + '()')

        # set input curves
        for name, curve in inputCurvesDict.iteritems():
            fitting.SetInputCurve(name, curve)

        # set initial optimization parameters
        nameList = fitting.GetOptimParamNameList()
        n = len(nameList)
        paramList = numpy.zeros(n)
        for i in range(n):
            paramList[i] = initialOptimParamDict[nameList[i]]

        fitting.SetTargetCurve(targetCurve)
        fitting.SetInitialOptimParam(paramList)

        fitting.Execute()
        x = outputCurve[:, 0]
        y = fitting.GetFitCurve(x)
        
        outputCurve[:, 1] = y
        
        result = fitting.GetOutputParam()

        return result
