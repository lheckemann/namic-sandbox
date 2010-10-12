import numpy as np
S = np.load('/Users/malcolm/src/dmri/01045_S.npy')
u = np.load('/Users/malcolm/src/dmri/01045_u.npy'); u = np.vstack((u,-u))
b = 900.
mask = np.load('/Users/malcolm/src/dmri/01045_mask.npy').astype('uint16')
seeds = np.load('/Users/malcolm/src/dmri/01045_seeds_tc.npy')
execfile('/Users/malcolm/src/filtered_tractography/Source/filtered2t.py')
ff = init(S, seeds, u, b, param)
