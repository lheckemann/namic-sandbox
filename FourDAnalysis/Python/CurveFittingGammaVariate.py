from FourDAnalysis import CurveAnalysisBase

import scipy, numpy
from scipy.integrate import quad

class CurveFittingGammaVariate(CurveAnalysisBase):

    def __init__(self):
        self.OptimParamNameList = ['Sp', 'alpha', 'beta', 'Ta', 'S0']
        self.InitialOptimParam  = [200.0, 3.0,    1.0,    0.0,  20.0] 
        
    def Function(self, x, param):
        Sp, alpha, beta, Ta, S0 = param
        y = Sp * numpy.abs(scipy.power((scipy.e / (alpha*beta)), alpha)) * numpy.abs(scipy.power((x-Ta), alpha)) * scipy.exp(-(x-Ta)/beta) + S0
        return y
    
    def GetOutputParam(self):
        Sp, alpha, beta, Ta, S0 = self.OptimParam

        sts = quad(lambda x: x*(self.Function(x, self.OptimParam) - S0), 0.0, 100.0)
        ss  = quad(lambda x: self.Function(x, self.OptimParam) - S0, 0.0, 100.0)
        MTT = sts[0] / ss[0]

        dict = {}
        dict['MTT']   = MTT
        dict['Sp']    = Sp
        dict['alpha'] = alpha
        dict['beta']  = beta
        dict['Ta']    = Ta
        dict['S0']    = S0

        return dict





