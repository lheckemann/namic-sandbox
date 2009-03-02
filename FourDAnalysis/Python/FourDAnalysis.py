import scipy.optimize 

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

    def SetInitialOptimParam(self, param):
        self.InitialOptimParam = param

    def GetInitialOptimParam(self):
        return self.InitialOptimParam
    
    def GetOptimParam(self):
        return self.OptimParam
    
    def SetSourceCurve(self, sourceCurve):
        self.SourceCurve = sourceCurve

    def Execute(self):
        x      = self.SourceCurve[:, 0]
        y_meas = self.SourceCurve[:, 1]

        param0 = self.InitialOptimParam
        param_output = scipy.optimize.leastsq(self.ResidualError, param0, args=(y_meas, x),full_output=True)
        self.OptimParam       = param_output[0] # fitted parameters
        self.CovarianceMatrix = param_output[1] # covariant matrix
        
        return 1        ## shoud return 0 if optimization fails

    def GetFitCurve(self, x):
        return self.Function(x, self.OptimParam)

    def GetOutputParam(self):   # returns dictionary
        return {}
