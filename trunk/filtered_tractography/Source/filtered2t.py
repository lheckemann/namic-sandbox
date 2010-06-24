XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Tractography</category>
  <title>Filtered: Two Tensor</title>
  <description>Filtered tractography with equal-weight two-tensor model.</description>



  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>

    <image type="diffusion-weighted">
      <name>dwi_node</name> <channel>input</channel> <index>0</index>
      <label>Input diffusion signal</label>
    </image>
    <image type="label">
      <name>seeds_node</name> <channel>input</channel> <index>1</index>
      <label>Input seed region</label>
    </image>
    <image type="label">
      <name>mask_node</name> <channel>input</channel> <index>2</index>
      <label>Input brain mask</label>
    </image>

    <geometry>
      <name>ff_node</name> <channel>output</channel> <index>3</index>
      <label>Output fiber paths</label>
    </geometry>
  </parameters>



  <parameters>
    <label>Settings</label>
    <description>Settings effecting fiber tracing</description>

    <float>
      <name>FA_min</name> <channel>input</channel> <index>5</index>
      <label>Fractional anisotropy minimum</label>
      <description>Tracing stops if the estimated FA drops below this threshold</description>
      <default>0.15</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>GA_min</name> <channel>input</channel> <index>6</index>
      <label>Generalized anisotropy minimum</label>
      <description>Tracing stops if the estimated GA drops below this threshold</description>
      <default>0.10</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>min_radius</name> <channel>input</channel> <index>7</index>
      <label>Curving radius minimum</label>
      <description>Tracing stops if the curve radius drops this threshold</description>
      <default>0.87</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>dt</name> <channel>input</channel> <index>8</index>
      <label>Time step (dt)</label>
      <description>Forward Euler time step</description>
      <default>0.2</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <integer>
      <name>seeds</name> <channel>input</channel> <index>9</index>
      <label>Seeds per voxel</label>
      <description>Number of random seeds to initialize in each voxel.  Deterministic between each run, i.e. run twice with same settings and get exact same tracts.</description>
      <default>5</default>
      <constraints> <minimum>0</minimum> </constraints>
    </integer>
    <integer>
      <name>max_len</name> <channel>input</channel> <index>10</index>
      <label>Maximum fiber length</label>
      <description>Tracing stops if the fiber length goes beyond this threshold</description>
      <default>250</default>
      <constraints> <minimum>0</minimum> </constraints>
    </integer>
    <float>
      <name>Qm</name> <channel>input</channel> <index>11</index>
      <label>Eigenvector sensitivity (Qm)</label>
      <description>Higher: orientation changes faster.  Lower: smoother paths.</description>
      <default>0.0030</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>Ql</name> <channel>input</channel> <index>12</index>
      <label>Eigenvalue sensitivity (Ql)</label>
      <description>Higher: tensor changes shape.  Lower: adapts to measurements slower.</description>
      <default>100</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>Rs</name> <channel>input</channel> <index>13</index>
      <label>Signal sensitivity (Rs)</label>
      <description>Higher: adapts to signal measurements faster.  Lower: relies on model estimates more.</description>
      <default>0.015</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>P0</name> <channel>input</channel> <index>14</index>
      <label>Initial covariance scale (P0)</label>
      <description>Level of uncertainty at start.  No effect after a few steps.</description>
      <default>0.01</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
  </parameters>
</executable>
"""

import numpy as np
import warnings
import filtered_ext as flt
import time

param = dict({'FA_min': .15,  # FA stopping threshold
              'GA_min': .1,  # generalized anisotropy stopping threshold
              'dt': .2,     # forward Euler step size (path integration)
              'max_len': 250, # stop if fiber gets this long
              'min_radius': .87,  # stop if fiber curves this much
              'seeds': 10, # how many seeds to spawn in each ROI voxel
              'voxel': np.mat('1.70; 1.66; 1.66'), # voxel size (check your .nhdr file)
              # Kalman filter parameters
              'Qm': .0030,  # injected angular noise (probably leave untouched)
              'Ql': 100,    # injected eigenvalue noise (probably leave untouched)
              'Rs': .015,  # dependent on latent noise in your data (likely change this)
              'P0': np.eye(10) / 100,}) # initial covariance (likely change this)

# using this or the above had little effect
# P0 = np.mat(' 0.0076   -0.0001   -0.0000   -0.0002    0.0002;\
#              -0.0001    0.0059    0.0003   -0.0003    0.0003;\
#              -0.0000    0.0003    0.0065    0.0001    0.0004;\
#              -0.0002   -0.0003    0.0001  690.6269  -75.2103;\
#               0.0002    0.0003    0.0004  -75.2103  355.8088')
# param['P0'][:] = 0;
# param['P0'][0:5,0:5] = P0
# param['P0'][5:10,5:10] = P0

def Execute(dwi_node, seeds_node, mask_node, ff_node, FA_min, GA_min, min_radius, dt, seeds, max_len, Qm, Ql, Rs, P0):
    for i in range(10) : print ''

    param = dict({'FA_min': FA_min, # fractional anisotropy stopping threshold
                  'GA_min': GA_min, # generalized anisotropy stopping threshold
                  'dt': dt,     # forward Euler step size (path integration)
                  'max_len': max_len, # stop if fiber gets this long
                  'min_radius': min_radius,  # stop if fiber curves this much
                  'seeds': seeds, # how many seeds to spawn in each ROI voxel
                  # Kalman filter parameters
                  'Qm': Qm,  # injected angular noise (probably leave untouched)
                  'Ql': Ql,    # injected eigenvalue noise (probably leave untouched)
                  'Rs': Rs,  # dependent on latent noise in your data (likely change this)
                  'P0': P0 * np.eye(10)}) # initial covariance (likely change this)

    from Slicer import slicer
    scene = slicer.MRMLScene
    dwi_node = scene.GetNodeByID(dwi_node)
    seeds_node = scene.GetNodeByID(seeds_node)
    mask_node = scene.GetNodeByID(mask_node)
    ff_node = scene.GetNodeByID(ff_node)

    # gather inputs
    S = dwi_node.GetImageData().ToArray()
    u = dwi_node.GetDiffusionGradients().ToArray()
    u = np.vstack((u,-u)) # duplicate signal
    b = dwi_node.GetBValues().ToArray().mean()
    mask  = mask_node.GetImageData().ToArray().astype('uint16')
    seeds = seeds_node.GetImageData().ToArray()

    # gather transformations
    mf  = vtk2mat(dwi_node.GetMeasurementFrameMatrix, slicer)
    r2i = vtk2mat(dwi_node.GetRASToIJKMatrix, slicer)
    i2r = vtk2mat(dwi_node.GetIJKToRASMatrix, slicer)
    param['voxel'] = np.mat(dwi_node.GetSpacing())[::-1].reshape(3,1)

    # tractography...
    ff = init(S, seeds, u, b, param)
    t1 = time.time()
    for i in range(0,len(ff)):
        print '[%3.0f%%] (%7d - %7d)' % (100.0*i/len(ff), i, len(ff))
        ff[i] = follow(S,u,b,mask,ff[i],param)
    print 'time: ', time.time() - t1

    # build polydata
    pts = slicer.vtkPoints()
    lines = slicer.vtkCellArray()
    cell_id = 0
    for i in range(0,len(ff)):  # TODO xrange
        f = ff[i]
        lines.InsertNextCell(len(f))
        for j in range(0,len(f)):
            lines.InsertCellPoint(cell_id)
            cell_id += 1
            x = f[j]
            x = x[::-1] # HACK
            x_ = np.array(transform(i2r, x)).ravel() # HACK
            pts.InsertNextPoint(x_[0],x_[1],x_[2])

    # setup output fibers
    dnode = ff_node.GetDisplayNode()
    if not dnode:
        dnode = slicer.vtkMRMLModelDisplayNode()
        ff_node.GetScene().AddNodeNoNotify(dnode)
        ff_node.SetAndObserveDisplayNodeID(dnode.GetID())
    pd = ff_node.GetPolyData()
    if not pd:
        pd = slicer.vtkPolyData() # create if necessary
        ff_node.SetAndObservePolyData(pd)
    pd.SetPoints(pts)
    pd.SetLines(lines)
    pd.Update()
    ff_node.Modified()

    asdf # HACK flush stdio


def vtk2mat(vtk_fn, sl):
    # get data from slicer
    v = sl.vtkMatrix4x4()
    vtk_fn(v)
    # push into numpy
    m = np.empty((4,4))
    for i,j in np.ndindex(4,4):
        m[i,j] = v.GetElement(i,j)

    return np.mat(m)  # HACK


def transform(M, v):
    v_ = M[0:3,0:3] * v
    return v_ + M[0:3,-1]

def step(p, S, est, param):
    v = param['voxel']
    # unpack
    x,X,P,m = p[0],p[1],p[2],p[3]  # ignore FA
    # estimate
    X,P = est(X,P,interp3signal(S,x,v))
    m1,l1,m2,l2 = state2tensor2(X,m)
    # pick most consistent direction
    if dot(m1,m) >= dot(m2,m):   m = m1; l = l1;
    else:                        m = m2; l = l2;
    fa = iff(l[0] < l[1], 0, l2fa(l))  # ensure eliptic
    # move
    dx = m / v
    x = x + param['dt'] * dx[::-1]  # HACK volume dimensions are reversed
    # repack
    return x,X,P,m,fa

def tensor2fa(T):
    T_ = np.trace(T)/3
    I = np.eye(3,3)
    return np.sqrt(1.5) * np.linalg.norm(T - T_*I,2) / np.linalg.norm(T,2)


def curve_radius(ff):
   if len(ff) < 3:
       return 1

   a,b,c = ff[-1],ff[-2],ff[-3]

   v1,v2 = b-c,a-b

   v1_ = norm(v1)
   v2_ = norm(v2)
   u1 = v1/v1_
   u2 = v2/v2_

   curv = norm(2 * (u2-u1)/(v2_ + v1_))
   if np.isnan(curv):
       return 1

   return 1/curv

def norm(a):
    return np.linalg.norm(a)


def follow(S,u,b,mask,fiber,param):
    # unpack and initialize tract
    x,X,P = fiber[0],fiber[1],fiber[2]
    ff = [np.array(x)]

    # initialize filter
    f_fn,h_fn = model_2tensor(u,b)
    Qm = param['Qm']*np.eye(3,3)
    Ql = param['Ql']*np.eye(2,2)
    Rs = param['Rs']*np.eye(u.shape[0],u.shape[0])
    Q = np.zeros((10,10))
    Q[0:3,0:3],Q[3:5,3:5] = Qm,Ql
    Q[5:8,5:8],Q[8:10,8:10] = Qm,Ql
    est = filter_ukf(f_fn,h_fn,Q,Rs)

    # main loop
    ct = 0
    v = param['voxel']
    while True :
        # estimate
        fiber = step(fiber, S, est, param)

        # unpack
        x,X,fa = fiber[0],fiber[1],fiber[4]

        # terminate if off brain or in CSF
        is_brain = interp3scalar(mask,x,v) > .1
        ga = s2ga(h_fn(X))
        is_csf = ga < param['GA_min'] or fa < param['FA_min']
        is_curving = curve_radius(ff) < param['min_radius']

        if not is_brain or is_csf or len(ff) > param['max_len'] or is_curving :
            if len(ff) > param['max_len'] : warnings.warn('wild fiber')
            return ff

        # record roughly once per voxel
        if ct == round(1.0/param['dt']):
            ct = 0
            ff.append(x)
        else:
            ct += 1


def init(S, seeds, u, b, param):
    print 'initial seed voxels: %d' % len(seeds.nonzero())

    # generate random unit directions
    np.random.seed(0) # determinism
    E = np.random.rand(3,param['seeds'])
    E = E / np.sqrt(np.sum(E**2,axis=0)) / 2  # half unit
    
    # perturb each index with those directions
    qq = zip(*seeds.nonzero())
    pp = []
    map(lambda p : map(lambda e : pp.append(p+e), list(E.T)), qq)

#     print 'HACK...'
#     pp = [np.array((27.0,69.0,72.0))] # HACK

    print 'initial seed upper limit: %d' % (2*len(pp))
    # indices -> signals -> tensors
    v = param['voxel']
    ss = map(lambda p : interp3signal(S,p,v), pp)
    dd = map(direct(u,b), ss)

    # {position, direction, lambda, covariance, FA}
    mmll = map(unpack_tensor, dd)
    P0 = np.mat(param['P0'])
    ff = map(lambda p,(m,l) : (p,m,l,P0,iff(l[0]>=l[1],l2fa(l),0)), pp, mmll)

    # filter out only high-FA tensors
    fa_min = param['FA_min']
    ff = filter(lambda f : f[4] > fa_min, ff)

    # add in all opposite directions
    ff.extend(map(lambda f : (f[0],-f[1],f[2],f[3],f[4]), ff))

    # convert to column vectors
    ff = map(lambda x : (np.mat(x[0]).T,                                         # position
                         np.mat(np.hstack((x[1],x[2],x[1],x[2]))).reshape(10,1), # state
                         x[3],                                                   # cov
                         x[1],                                                   # direction
                         x[4]),                                                  # FA
             ff)

    return ff


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



def filter_ukf(f_fn,h_fn,Q,R):
    n = Q.shape[0]
    k = .01

    scale = np.sqrt(n + k)
    w = np.vstack((k,np.tile(.5,(2*n,1)))) / (n+k)
    w_ = np.diag(w.ravel())
    w,w_ = np.mat(w),np.mat(w_)

    def sigma_points(x,P,scale):
        M = scale * np.linalg.cholesky(P); # faster than mroot since symmetric pos def
        X = np.tile(x,len(x));
        X = np.hstack((x,X+M, X-M))
        return X

    def filter(x,P,z):
        ##-- (1) create sigma points
        X = sigma_points(x, P, scale)

        ##-- (2) predict state (also its mean and covariance)
        X = f_fn(X)
        x_hat = X * w
        X_ = X - np.tile(x_hat, (1,2*n+1)) # center
        P = X_ * w_ * X_.T + Q # partial update

        ##-- (3) predict observation (also its mean and covariance)
        Y = h_fn(X)
        Y_hat = Y * w
        Y_ = Y - np.tile(Y_hat, (1,2*n+1)) # center
        Pyy = Y_ * w_ * Y_.T + R

        ##-- (4) predict cross-correlation between state and observation
        Pxy = X_ * w_ * Y_.T

        ##-- (5) Kalman gain K, estimate state/observation, compute cov.
        K = np.linalg.solve(Pyy, Pxy.T)
        P = P - K.T * Pyy * K
        z = z.reshape(Pyy.shape[0],1)
        x = x_hat + K.T * (z - Y_hat)

        return x,P

    return filter



def model_2tensor(u,b):
    f_fn = model_2tensor_f   # identity, but fix up state
    h_fn = lambda X : model_2tensor_h(X,u,b)
    return f_fn,h_fn

def model_2tensor_f(X):
    assert X.shape[0] == 10 and X.dtype == 'float64'
    m = X.shape[1]
    X = np.copy(X)
    flt.c_model_2tensor_f(X, m)
    return X

def model_2tensor_h(X,u,b):
    assert X.shape[0] == 10 and X.dtype == 'float64' and u.dtype == 'float64'
    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n,m), order='F') # preallocate output
    flt.c_model_2tensor_h(s, X, u, b, n, m)
    return s

def state2tensor2(X, y=np.zeros(3)):
    assert X.shape[0] == 10 and X.shape[1] == 1 and X.dtype == 'float64'
    m1,m2 = np.empty((3,1)),np.empty((3,1))
    l11,l12 = np.empty(1),np.empty(1);
    l21,l22 = np.empty(1),np.empty(1);
    flt.c_state2tensor2(X, y, m1,l11,l12, m2,l21,l22)
    return m1,(l11,l12), m2,(l21,l22)




def l2fa((a,b)):
    return abs(a-b)/np.sqrt(a*a + 2*b*b)

def dot(a,b):
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]



def s2ga(s):
    assert s.dtype == 'float64'
    n = s.shape[0]
    return flt.c_s2ga(s,n)


def interp3signal(S, p, v=np.ones(3)):
    assert S.ndim == 4 and S.dtype == 'float32'
    nx,ny,nz,n = S.shape
    s = np.zeros((2*n,), dtype='float32')  # preallocate output (doubled)
    flt.c_interp3signal(s, S, p, v, nx, ny, nz, n)
    return s


def interp3scalar(M, p, v=np.ones(3)):
    assert M.ndim == 3 and M.dtype == 'uint16'
    nx,ny,nz = M.shape
    return flt.c_interp3scalar(M, p, v, nx, ny, nz)

# ternary operator (no short circuit)
def iff(test, true, false):
    if test:  return true
    else:     return false
