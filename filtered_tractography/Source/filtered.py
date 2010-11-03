XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>
  <category>Diffusion.Tractography</category>
  <title>UKF Tractography</title>
  <description>Filtered tractography with either one-tensor or equal-weight two-tensor model. The module also lets you switch between a simple tensor representation where the 2nd and 3rd eigenvalues are assumed to be the same and a full representation where the 2nd and 3rd eigenvalues can be different. The parameters Qm, Ql and Rs allow to change the behavior of the filter. Qm and determine how much variance is allowed in the model. Rs governs how much variance is expected in the measurment: Higher values mean we expect high variance and hence trust our measurment less. We found that Qm = 0.003, Ql = 100, Rs = 0.015 works well for the simple tensor representation, and Qm = 0.002, Ql = 200, Rs = 0.01 works best for the full tensor representation. The default values are currently set to the opitmal values for the simple tensor representation. The record check button allow to store additional data together with the VTK object. The data will be written the the .vtk file as point data.</description> 
  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>
    <image type="diffusion-weighted">
      <name>dwi_node</name> <longflag>dwi_node</longflag> <channel>input</channel>
      <label>Input diffusion signal</label>
    </image>
    <image type="label">
      <name>seeds_node</name> <longflag>seeds_node</longflag> <channel>input</channel>
      <label>Input seed region</label>
    </image>
    <image type="label">
      <name>mask_node</name> <longflag>mask_node</longflag> <channel>input</channel>
      <label>Input brain mask</label>
    </image>
    <geometry>
      <name>ff_node</name> <longflag>ff_node</longflag> <channel>output</channel>
      <label>Output fiber paths</label>
    </geometry>
    <boolean>
      <name>record_fa</name> <longflag>record_fa</longflag> <channel>input</channel>
      <label>Record FA</label>
      <default>false</default>
    </boolean>
    <boolean>
      <name>record_state</name> <longflag>record_state</longflag> <channel>input</channel>
      <label>Record state</label>
      <default>false</default>
    </boolean>
    <boolean>
      <name>record_cov</name> <longflag>record_cov</longflag> <channel>input</channel>
      <label>Record covariance</label>
      <default>false</default>
    </boolean>
  </parameters>
  <parameters>
    <label>Settings</label>
    <description>Settings effecting fiber tracing</description>
    <string-enumeration>
      <name>model</name><longflag>model</longflag> <channel>input</channel>
      <description>Model</description>
      <label>Which voxel model to use</label>
      <element>one-tensor</element>
      <element>two-tensor</element>
      <default>two-tensor</default>
    </string-enumeration>
    <string-enumeration>
      <name>tensor_model</name>
      <longflag>tensor_model</longflag>
      <channel>input</channel>
      <description>The simple tensor model assumes that the 2nd and 3rd eigenvalues are identical whereas in the full model the 3rd eigenvalue can differ from the 2nd.</description>
      <label>Tensor model</label>
      <element>Full</element>
      <element>Simple</element>
      <default>Simple</default>
    </string-enumeration>
    <float>
      <name>FA_min</name> <longflag>FA_min</longflag> <channel>input</channel>
      <label>Fractional anisotropy minimum</label>
      <description>Tracing stops if the estimated FA drops below this threshold</description>
      <default>0.15</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>GA_min</name> <longflag>GA_min</longflag> <channel>input</channel>
      <label>Generalized anisotropy minimum</label>
      <description>Tracing stops if the estimated GA drops below this threshold</description>
      <default>0.10</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <integer>
      <name>seeds</name> <longflag>seeds</longflag> <channel>input</channel>
      <label>Seeds per voxel</label>
      <description>Number of random seeds to initialize in each voxel.  Deterministic between each run, i.e. run twice with same settings and get exact same tracts.</description>
      <default>1</default>
      <constraints> <minimum>0</minimum> </constraints>
    </integer>
    <integer-vector>
      <name>labels</name> <longflag>labels</longflag> <channel>input</channel>
      <label>Labels used for seeding</label>
      <description>The values of the labels indicating voxels to seed.  Accepts multiple comma-separated values.</description>
      <default>1</default>
    </integer-vector>
    <float>
      <name>Qm</name> <longflag>Qm</longflag> <channel>input</channel>
      <label>Eigenvector sensitivity (Qm)</label>
      <description>Higher: orientation changes faster.  Lower: smoother paths.</description>
      <default>0.0030</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>Ql</name> <longflag>Ql</longflag> <channel>input</channel>
      <label>Eigenvalue sensitivity (Ql)</label>
      <description>Higher: tensor changes shape.  Lower: adapts to measurements slower.</description>
      <default>100</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <float>
      <name>Rs</name> <longflag>Rs</longflag> <channel>input</channel>
      <label>Signal sensitivity (Rs)</label>
      <description>Higher: adapts to signal measurements faster.  Lower: relies on model estimates more.</description>
      <default>0.015</default>
      <constraints> <minimum>0</minimum> </constraints>
    </float>
    <integer>
      <name>theta_max</name> <longflag>theta_max</longflag> <channel>input</channel>
      <label>Maximum branching angle (degrees)</label>
      <description>Ignore branching if angle above this.  Zero if desire no branching.</description>
      <default>0</default>
      <constraints> <maximum>90</maximum> </constraints>
    </integer>
  </parameters>
</executable>
"""

import numpy as np
import warnings
import filtered_ext as flt
import time
import math
import itertools

def Execute(dwi_node, seeds_node, mask_node, ff_node, \
            record_fa = False, record_state = True, record_cov = False, \
            model="two-tensor", tensor_model="Simple", FA_min=.15, GA_min=.10,
            seeds=1, labels=[1],
            Qm=.0030, Ql=100, Rs=.015, theta_max=0):
    for i in xrange(10) : print ''

    is_2t = (model == "two-tensor")
    follow = iff(is_2t, follow2t, follow1t)

    is_model_full = (tensor_model == "Full")

    # Dimension of state space.
    state_dim = iff(is_model_full, iff(is_2t, 12, 6), iff(is_2t, 10, 5))

    dt = iff(is_2t, 0.2,    0.3)
    #Qm = iff(is_2t, 0.0015, 0.0015)
    #Ql = iff(is_2t, 100,    25)
    #Rs = iff(is_2t, 0.015, 0.020)

    theta_min = 5  # angle which triggers branch
    param = dict({'FA_min': FA_min, # fractional anisotropy stopping threshold
                  'GA_min': GA_min, # generalized anisotropy stopping threshold
                  'dt': dt,     # forward Euler step size (path integration)
                  'max_len': 250, # stop if fiber gets this long
                  'min_radius': .87,  # stop if fiber curves this much
                  'seeds' : seeds,  # number of seeds in each voxel
                  'theta_min': np.cos(theta_min * np.pi/180),  # angle which triggers branch
                  'theta_max': np.cos(theta_max * np.pi/180),  # angle which triggers branch
                  'min_radius': .87,  # stop if fiber curves this much
                  'record_fa' : record_fa,  # not necessary, could be inferred from state
                  'record_st' : record_state,
                  'record_co' : record_cov,
                  # Kalman filter parameters
                  'Qm': Qm, # injected angular noise
                  'Ql': Ql, # injected eigenvalue noise
                  'Rs': Rs, # dependent on latent noise in your data
                  'P0': 0.01 * np.eye(state_dim)}) # initial covariance

    from Slicer import slicer

    scene = slicer.MRMLScene
    dwi_node = scene.GetNodeByID(dwi_node)
    seeds_node = scene.GetNodeByID(seeds_node)
    mask_node = scene.GetNodeByID(mask_node)
    ff_node = scene.GetNodeByID(ff_node)

    # ensure normalized signal
    bb = dwi_node.GetBValues().ToArray()
    if np.any(bb.sum(1) == 0):
        import Slicer
        Slicer.tk.eval('tk_messageBox -message "Use the \'Normalize Signal\' module to prepare the DWI data"')
        return
    b = bb.mean()

    S = dwi_node.GetImageData().ToArray()
    u = dwi_node.GetDiffusionGradients().ToArray()
    i2r = vtk2mat(dwi_node.GetIJKToRASMatrix,         slicer)
    r2i = vtk2mat(dwi_node.GetRASToIJKMatrix,         slicer)
    mf  = vtk2mat(dwi_node.GetMeasurementFrameMatrix, slicer)
    voxel = np.mat(dwi_node.GetSpacing()).reshape(3,1)

    voxel = voxel[::-1]  # HACK Numpy has [z y x]

    # generalized loading...
    R = r2i[0:3,0:3] / voxel.T  # normalize each column
    M = mf[0:3,0:3]

    # transform gradients
    u = dwi_node.GetDiffusionGradients().ToArray()
    u = u * (np.linalg.inv(R) * M).T
    u = u / np.sqrt(np.power(u,2).sum(1))
    u = np.vstack((u,-u)) # duplicate signal

    param['voxel'] = np.array(voxel)
    #param['voxel'] = voxel
    mask  = mask_node.GetImageData().ToArray().astype('uint16')

    # pull all seeds
    seeds_ = seeds_node.GetImageData().ToArray()
    seeds = np.zeros(seeds_.shape, dtype='bool')
    for lbl in labels:
        seeds |= (seeds_ == lbl)
    seeds = zip(*seeds.nonzero())


    # ensure seeds
    if len(seeds) == 0:
        import Slicer
        Slicer.tk.eval('tk_messageBox -message "No seeds found for this label"')
        return

    # double check branching
    if 0 < theta_max and theta_max < 5:
        import Slicer
        Slicer.tk.eval('tk_messageBox -message "Nonzero branching angle must be greater than 5 degrees"')
        return
    is_branching = is_2t and param['theta_max'] < 1

    # tractography...
    ff1 = init(S, seeds, u, b, param, is_2t, is_model_full)

    ff2,ff_fa,ff_st,ff_co = [],[],[],[]
    t1 = time.time()
    for i in xrange(0,len(ff1)):
        print '[%3.0f%%]p (%7d - %7d)' % (100.0*i/len(ff1), i, len(ff1))
        ff1[i],next,extra = follow(S,u,b,mask,ff1[i],param,is_branching)
        ff2.extend(next)
        # store extras
        if record_fa:    ff_fa.append(extra[0])
        if record_state: ff_st.append(extra[1])
        ff_co.append(extra[2])

    t2 = time.time()
    print 'Time: ', t2 - t1, 'sec'

    if is_branching:
        for i in xrange(0,len(ff2)):
            print '[%3.0f%%]s (%7d - %7d)' % (100.0*i/len(ff2), i, len(ff2))
            f,_,extra = follow(S,u,b,mask,ff2[i],param,False)
            ff1.append(f)
            # store extras
            if record_fa:    ff_fa.append(extra[0])
            if record_state: ff_st.append(extra[1])
            ff_co.append(extra[2])
        print 'Time: ', time.time() - t2, 'sec'

    # build polydata
    num_points = 0
    for f in ff1:
        num_points += len(f)
    ss_x = slicer.vtkPoints()
    ss_x.SetNumberOfPoints(num_points)
    lines = slicer.vtkCellArray()

    ss_norm = slicer.vtkFloatArray()
    ss_norm.SetName('norm')
    ss_norm.SetNumberOfComponents(1)
    ss_norm.SetNumberOfTuples(num_points)
    ss_norm_arr = ss_norm.ToArray()

    if record_fa:
        ss_fa = slicer.vtkFloatArray()
        ss_fa.SetNumberOfComponents(1)
        ss_fa.SetNumberOfTuples(num_points)
        ss_fa.SetName('FA')
        ss_fa_arr = ss_fa.ToArray()

    if record_state:
        ss_st = slicer.vtkFloatArray()
        ss_st.SetNumberOfComponents(state_dim)
        ss_st.SetNumberOfTuples(num_points)
        ss_st.SetName('state');
        ss_st_arr = ss_st.ToArray()

    if record_cov:
        upper_half_coordinates = tuple(np.transpose([(i,j) \
            for i in xrange(state_dim) \
            for j in xrange(i, state_dim)]))
        ss_co = slicer.vtkFloatArray()
        ss_co.SetNumberOfComponents(state_dim * (state_dim + 1) / 2)
        ss_co.SetNumberOfTuples(num_points)
        ss_co.SetName('covariance')
        ss_co_arr = ss_co.ToArray()

    point_id = 0
    for i in xrange(0,len(ff1)):
        f = ff1[i]
        if record_fa:    f_fa = ff_fa[i]
        if record_state: f_st = ff_st[i]
        f_co = ff_co[i]
        lines.InsertNextCell(len(f))

        for j in xrange(0,len(f)):
            lines.InsertCellPoint(point_id)
            x = f[j]
            x = x[::-1] # HACK
            x_ = np.array(transform(i2r, x)).ravel() # HACK
            ss_x.SetPoint(point_id, x_[0], x_[1], x_[2])

            ss_norm_arr[point_id, 0] = np.linalg.norm(f_co[j])
            if record_fa:     ss_fa_arr[point_id, 0] = 255 * f_fa[j]
            if record_state:  ss_st_arr[point_id, :] = f_st[j].ravel()
            if record_cov:    ss_co_arr[point_id, :] = f_co[j][upper_half_coordinates]

            point_id += 1

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
    pd.SetPoints(ss_x)
    pd.GetPointData().AddArray(ss_norm)
    if record_fa:     pd.GetPointData().AddArray(ss_fa)
    else:             pd.GetPointData().RemoveArray('FA')
    if record_state:  pd.GetPointData().AddArray(ss_st)
    else:             pd.GetPointData().RemoveArray('state')
    if record_cov:    pd.GetPointData().AddArray(ss_co)
    else:             pd.GetPointData().RemoveArray('covariance')
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


def swap_state(X, P):
    dim = len(X) / 2
    P_ = P.copy()
    P_[:dim, :dim] = P[dim:, dim:] # Swap covariance.
    P_[dim:, dim:] = P[:dim, :dim] 
    P_[dim:, :dim] = P[:dim, dim:] 
    P_[:dim, dim:] = P[dim:, :dim]
    P = P_
    X = np.vstack((X[dim:2 * dim], X[:dim]))
    return X, P

def step2t(p, S, est, param):
    v = param['voxel']
    x, X, P, m = p[0], p[1], p[2], p[3]

    # Get new State.
    X, P = est(X, P, interp3signal(S, x, v))
    m1, l1, m2, l2 = state2tensor(X, m)

    # Pick most consistent direction.
    if dot(m1, m) >= dot(m2, m):
        dirs = (m1, l1, m2, l2)
        m = m1
    else:
        dirs = (m2, l2, m1, l1)
        m = m2
        X, P = swap_state(X, P)
    if len(X) == 12:
        fa = iff(dirs[1][0] < dirs[1][1] or dirs[1][0] < dirs[1][2], \
                 0, l2fa(dirs[1])) # Ensure eliptic.
    else: # len(X) == 10
        fa = iff(dirs[1][0] < dirs[1][1], 0, l2fa(dirs[1])) # Ensure eliptic.

    # Move along the found direction
    dx = dirs[0] / v
    x = x + param['dt'] * dx[::-1]  # HACK volume dimensions are reversed
    return x, X, P, m, fa, dirs


def step1t(p, S, est, param):
    v = param['voxel']
    x,X,P = p[0],p[1],p[2]
    # Get new state.

    X,P = est(X,P,interp3signal(S,x,v))

    m, l = state2tensor(X)
    if len(X) == 6:
        fa = iff(l[0] < l[1] or l[0] < l[2], 0, l2fa(l)) # Ensure eliptic.
    else: # len(X) == 5
        fa = iff(l[0] < l[1], 0, l2fa(l)) # Ensure eliptic.
    # Move along the found direction
    dx = m / v
    x = x + param['dt'] * dx[::-1]  # HACK volume dimensions are reversed
    return x,X,P,m,fa


# TODO: not used?
#def tensor2fa(T):
#    T_ = np.trace(T)/3
#    I = np.eye(3,3)
#    return np.sqrt(1.5) * np.linalg.norm(T - T_*I,2) / np.linalg.norm(T,2)


def curve_radius(ff):
   if len(ff) < 3:
       return 1

   a,b,c = ff[-1],ff[-2],ff[-3]

   v1,v2 = b-c,a-b

   v1_ = np.linalg.norm(v1)
   v2_ = np.linalg.norm(v2)
   u1 = v1/v1_
   u2 = v2/v2_

   curv = np.linalg.norm(2 * (u2-u1)/(v2_ + v1_))
   if np.isnan(curv):
       return 1

   return 1/curv


def follow1t(S,u,b,mask,fiber,param,is_branching):
    x,X,P,fa = fiber
    ff,ff_fa,ff_st,ff_co = [np.array(x)],[fa],[X],[P]
    pp = []

    if len(X) == 6:
        num_ev = 3
    else:
        num_ev = 2

    # initialize filter
    f_fn, h_fn = model_tensor(u, b)
    Qm = param['Qm']*np.eye(3,3)
    Ql = param['Ql']*np.eye(num_ev,num_ev)
    Rs = param['Rs']*np.eye(u.shape[0],u.shape[0])
    Q = np.zeros((3 + num_ev,3 + num_ev))
    Q[0:3,0:3],Q[3:3 + num_ev,3:3 + num_ev] = Qm,Ql
    est = filter_ukf(f_fn,h_fn,Q,Rs)

    stepnr = 0

    # main loop
    ct = 0
    v = param['voxel']
    while True :
        stepnr += 1

        # estimate
        fiber = step1t(fiber, S, est, param)

        # unpack
        x,X,P,m,fa = fiber

        # terminate if off brain or in CSF
        is_brain = interp3scalar(mask,x,v) > .1
        ga = s2ga(h_fn(X))
        is_csf = ga < param['GA_min'] or fa < param['FA_min']
        is_curving = curve_radius(ff) < param['min_radius']

        if not is_brain or is_csf or len(ff) > param['max_len'] or is_curving :
            if len(ff) > param['max_len'] : warnings.warn('wild fiber')
            return ff,pp,(ff_fa,ff_st,ff_co)

        # record roughly once per voxel
        if ct == round(1.0/param['dt']):
            ct = 0
            ff.append(x)
            if param['record_fa'] : ff_fa.append(fa)
            if param['record_st'] : ff_st.append(X)
            ff_co.append(P)
        else:
            ct += 1


def follow2t(S,u,b,mask,fiber,param,is_branching):
    # unpack and initialize tract
    x, X, P, m, fa = fiber
    ff, ff_fa, ff_st, ff_co = [np.array(x)], [fa], [np.vstack(X)], [P]
    pp = [] # branches if any

    if len(X) == 12:
        ne = 3
    else:
        ne = 2
    hd = 3 + ne

    # initialize filter
    f_fn, h_fn = model_tensor(u, b)
    Qm = param['Qm'] * np.eye(3, 3)
    Ql = param['Ql'] * np.eye(ne, ne)
    Rs = param['Rs'] * np.eye(u.shape[0], u.shape[0])
    Q = np.zeros((2 * hd, 2 * hd))
    Q[0:3, 0:3], Q[3:hd, 3:hd] = Qm, Ql
    Q[hd:(hd + 3), hd:(hd + 3)], Q[(hd + 3):(2 * hd), (hd + 3):(2 * hd)] = Qm, Ql
    est = filter_ukf(f_fn, h_fn, Q, Rs)

    stepnr = 0

    # main loop
    ct = 0
    v = param['voxel']
    while True:
        stepnr += 1

        # estimate
        fiber = step2t(fiber, S, est, param)
        x, X, P, m, fa, dirs = fiber

        # Terminate if off brain or in CSF.
        is_brain = interp3scalar(mask,x,v) > .1
        ga = s2ga(h_fn(X))
        is_csf = ga < param['GA_min'] or fa < param['FA_min']
        is_curving = curve_radius(ff) < param['min_radius']

        if not is_brain or is_csf or len(ff) > param['max_len'] or is_curving :
            if len(ff) > param['max_len'] : warnings.warn('wild fiber')
            return ff,pp,(ff_fa,ff_st,ff_co)

        # Record roughly once per voxel.
        if ct == round(1.0/param['dt']):
            ct = 0
            ff.append(x)
            if param['record_fa'] : ff_fa.append(fa)
            if param['record_st'] : ff_st.append(np.vstack(X))
            ff_co.append(P)

            # Record branch if necessary.
            if is_branching:
                m1, l1, m2, l2 = dirs
                fa = param['FA_min']
                if len(X) == 12:
                    is_two = l1[0] > l1[1] and l1[0] > l1[2] and \
                             l2[0] > l2[1] and l2[0] > l2[2]
                else: # len(X) == 10
                    is_two = l1[0] > l1[1] and l2[0] > l2[1] # non-planar
                is_two = is_two and l2fa(l1) > fa and l2fa(l2) > fa
                th = dot(m1, m2)
                is_branch = th < param['theta_min'] and th > param['theta_max']
                if is_two and is_branch:
                    # Swap orientations.
                    X, P = swap_state(X, P)
                    #pp.append((x, X, P, m2, fa))
                    pp.append((x, X, P, m2, l2fa(l2)))
        else:
            ct += 1



def init(S, seeds, u, b, param, is_2t, is_model_full):
    u = np.array(u)
    print 'initial seed voxels: %d' % len(seeds)

    # generate random unit directions
    np.random.seed(0) # determinism
    E = np.random.randn(3,param['seeds'])
    E = E / np.sqrt(np.sum(E**2,axis=0)) / 2  # half unit

    # perturb each index with those directions
    pp = []
    map(lambda p : map(lambda e : pp.append(p+e), list(E.T)), seeds)

    print 'initial seed upper limit: %d' % (2*len(pp))
    # indices -> signals -> tensors
    v = param['voxel']
    ss = map(lambda p : interp3signal(S,p,v), pp)

    # Filter out negative signal values and the corresponding points.
    bitset = map(lambda s: not any(itertools.imap(lambda x: x < 1e-06, s)) , ss)
    ss_tmp = []
    pp_tmp = []
    for i in xrange(len(bitset)):
        if bitset[i]:
            ss_tmp.append(ss[i]);
            pp_tmp.append(pp[i]);
    ss = ss_tmp
    pp = pp_tmp

    # Map signal to tensor.
    dd = map(direct(u,b), ss)

    mmll = map(unpack_tensor, dd)
    P0 = np.mat(param['P0'])
    if is_model_full:
        ff = map(lambda p, (m, angles, l) : (p, m, angles, l, P0, \
                                             l2fa(l)), pp, mmll)
    else:
        # Reduce the state to just two eigenvalues.
        ev2 = lambda x : (x[0], (x[1] + x[2]) / 2.)
        ff = map(lambda p, (m, angles, l) : (p, m, angles, ev2(l), P0, \
                                             l2fa(ev2(l))), pp, mmll)

    # Filter out only high-FA tensors.
    fa_min = param['FA_min']
    ff = filter(lambda f : f[5] > fa_min, ff)

    # Add in all opposite directions.
    switch_angles = lambda x: (x[0], x[1], \
                               iff(x[2] < 0, x[2] + math.pi, x[2] - math.pi))
    ff.extend(map(lambda f : (f[0], -f[1], switch_angles(f[2]), f[3], f[4], \
                              f[5]), ff))

    # Package state.
    ev2 = lambda x : (x[0], (x[1] + x[2]) / 2.)
    if is_2t:
        if not is_model_full:
            state = lambda x : np.mat(np.hstack( \
                    (x[1], x[3], x[1], x[3]))).reshape(10, 1)
        else:
            state = lambda x : np.mat(np.hstack( \
                    (x[2], x[3], x[2], x[3]))).reshape(12, 1)
        fiber = lambda x : (np.mat(x[0]).T, # position
                            state(x),       # state
                            x[4],           # cov
                            x[1],           # direction
                            x[5])           # FA
    else:
        if not is_model_full:
            state = lambda x : np.mat(np.hstack((x[1], x[3]))).reshape(5, 1)
        else:
            state = lambda x : np.mat(np.hstack((x[2], x[3]))).reshape(6, 1)
        fiber = lambda x : (np.mat(x[0]).T, # position
                            state(x),       # state
                            x[4],           # cov
                            x[5])           # FA
    return map(fiber, ff)


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


# Get direction, eigenvalues and euler angles from the tensor.
def unpack_tensor(d):
    D = d[[0, 1, 2, 1, 3, 4, 2, 4, 5]].reshape(3,3)
    Q,s,_ = np.linalg.svd(D)
    if np.linalg.det(Q) < 0:
        Q = -Q
    assert np.linalg.det(Q) > 0
    theta = math.acos(Q[2,2])

    # Extract the Euler Angles from the rotation matrix
    epsilon = 1e-10
    if abs(theta) > epsilon:
        phi = math.atan2(Q[1,2], Q[0,2])
        psi = math.atan2(Q[2,1], -Q[2,0])
    else:
        phi = math.atan2(-Q[0,1], Q[1,1])
        psi = 0

    m = Q[:,0]
    s *= 1e6
    return m, (theta, phi, psi), s


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


def f(X):
    assert X.dtype == 'float64'
    m = X.shape[1]
    X = np.copy(X)
    if X.shape[0] == 6:
        fn = flt.c_model_1tensor_full_f
    elif X.shape[0] == 12:
        fn = flt.c_model_2tensor_full_f
    elif X.shape[0] == 5:
        fn = flt.c_model_1tensor_f
    else: # X.shape[0] == 10
        fn = flt.c_model_2tensor_f
    fn(X, m)
    return X


def h(X, u, b):
    assert X.dtype == 'float64' and u.dtype == 'float64'
    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n, m), order='F') # preallocate output
    if X.shape[0] == 6:
        fn = flt.c_model_1tensor_full_h
    elif X.shape[0] == 12:
        fn = flt.c_model_2tensor_full_h
    elif X.shape[0] == 5:
        fn = flt.c_model_1tensor_h
    else: # X.shape[0] == 10
        fn = flt.c_model_2tensor_h
    fn(s, X, u, b, n, m)
    return s


def state2tensor(X, y=np.zeros(3)):
    assert X.shape[1] == 1 and X.dtype == 'float64'
    m1 = np.empty((3, 1))
    l11, l12 = np.empty(1), np.empty(1);
    if len(X) == 6:
        l13 = np.empty(1);
        flt.c_state2tensor1_full(X, y, m1, l11, l12, l13)
        return m1, (l11, l12, l13)
    elif len(X) == 12:
        m2 = np.empty((3, 1))
        l13 = np.empty(1)
        l21, l22, l23 = np.empty(1), np.empty(1), np.empty(1);
        flt.c_state2tensor2_full(X, y, m1, l11, l12, l13, m2, l21, l22, l23)
        return m1, (l11, l12, l13), m2, (l21, l22, l23)
    elif len(X) == 5:
        flt.c_state2tensor1(X, y, m1, l11, l12)
        return m1, (l11, l12)
    else: # len(X) == 10
        m2 = np.empty((3, 1))
        l21, l22 = np.empty(1), np.empty(1);
        flt.c_state2tensor2(X, y, m1, l11, l12, m2, l21, l22)
        return m1, (l11, l12), m2, (l21, l22)


def model_tensor(u, b):
    f_fn = f # Identity, but fix up state.
    h_fn = lambda X : h(X, u, b)
    return f_fn, h_fn


def l2fa(l):
   if len(l) == 2:
        return abs(l[0] - l[1]) / np.sqrt(l[0] * l[0] + 2 * l[1] * l[1])
   else:
        s = sum(l) / 3
        return np.sqrt(.5 * ((l[0] - s) * (l[0] - s) + \
                             (l[1] - s) * (l[1] - s) + \
                             (l[2] - s) * (l[2] - s)) / \
                       (l[0] * l[0] + l[1] * l[1] + l[2] * l[2]))


def dot(a,b):
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]


def s2ga(s):
    assert s.dtype == 'float64'
    n = s.shape[0]
    return flt.c_s2ga(s,n)


def interp3signal(S, p, v):
    assert S.ndim == 4 and S.dtype == 'float32'
    nx,ny,nz,n = S.shape
    sigma = 1.0 #1.66*1.66*1.66
    s = np.zeros((2*n,), dtype='float32')  # preallocate output (doubled)
    flt.c_interp3signal(s, S, p, np.array(v), sigma, nx, ny, nz, n)
    return s


def interp3scalar(M, p, v):
    assert M.ndim == 3 and M.dtype == 'uint16'
    nx,ny,nz = M.shape
    sigma = 1.0#1.66*1.66*1.66
    return flt.c_interp3scalar(M, p, np.array(v), sigma, nx, ny, nz)


# Ternary operator (no short circuit).
def iff(test, true, false):
    if test:  return true
    else:     return false
