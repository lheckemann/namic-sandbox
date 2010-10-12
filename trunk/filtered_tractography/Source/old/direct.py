import cProfile
import pstats

loc = dict({'S' : S,
            'u' : u,
            'b' : b,
            'ff'     : ff,
            'mask'   : mask,
            'param'  : param,
            'follow' : follow})
cProfile.runctx('follow(S,u,b,mask,ff[0],param)', dict(), loc, 'profile')
p = pstats.Stats('profile').strip_dirs()
p.sort_stats('time').print_stats(20)


execfile('/Users/malcolm/src/filtered_tractography/Source/direct.py')


s = S[28,72,72,:]
p = np.array([26., 72., 72.])


import warnings
import numpy as np

from scipy import weave

def tensor2fa(T):
    T_ = np.trace(T)/3
    I = np.eye(3,3)
    return np.sqrt(1.5) * np.linalg.norm(T - T_*I,2) / np.linalg.norm(T,2)


# calculate six tensor coefficients by solving B*d=log(s)
#   where d are tensor coefficients, B is gradient weighting, s is signal
def direct(u, b):
    # form the coefficient matrix
    x,y,z = u[:,0],u[:,1],u[:,2]
    B = -b * np.column_stack([x**2, 2*x*y, 2*x*z, y**2, 2*y*z, z**2])

    # closure
    def est(s):
        d,_,_,_ = np.linalg.lstsq(B, np.log(s))
        return d

    return est


# calculate the principal direction and eigenvalues from 6 coeff
def unpack_tensor(d):
    D = d[[0, 1, 2, 1, 3, 4, 2, 4, 5]].reshape(3,3)
    U,s,_ = np.linalg.svd(D)
    assert s[0] > s[1] and s[1] > s[2]

    m = U[:,0]
    s *= 1e6
    l = s[0], (s[1]+s[2])/2    # average two minor eigenvalues
    return m,l
