XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Tractography</category>
  <title>Python Streamline</title>
  <description>Streamline tractography.</description>

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
</executable>
"""

import numpy as np
import warnings
import cProfile

def Execute(dwi_node, seeds_node, mask_node, ff_node):
    from Slicer import slicer
    for i in range(10) : print ''
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
    mask  = mask_node.GetImageData().ToArray()
    seeds = seeds_node.GetImageData().ToArray()

    # gather transformations
    mf  = vtk2mat(dwi_node.GetMeasurementFrameMatrix)
    r2i = vtk2mat(dwi_node.GetRASToIJKMatrix)
    i2r = vtk2mat(dwi_node.GetIJKToRASMatrix)

    # tractography...
    param = dict({'FA_min': 0.1,
                  'dt': 0.3,
                  'max_len': 50,
                  'min_radius': 0.87,
                  'seeds': 1,
                  'voxel': np.array([1.66,1.66,1.70])})

    ff = init(S, seeds, u, b, param)
    n = len(ff)
    cell_id = 0
    for i in range(0,len(ff)):
        print '[%3.0f%%] (%7d - %7d)' % (100.0*i/n, i, n)
        ff[i] = follow(S,u,b,mask,ff[i],param)

    # build polydata
    pts = slicer.vtkPoints()
    lines = slicer.vtkCellArray()
    for i in range(0,len(ff)):
        f = ff[i]
        lines.InsertNextCell(len(f))
        for j in range(0,len(f)):
            lines.InsertCellPoint(cell_id)
            cell_id += 1
            x = f[j]
            x = x[::-1] # HACK
            x_ = transform(i2r, x)
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


def vtk2mat(vtk_fn):
    # get data from slicer
    v = slicer.vtkMatrix4x4()
    vtk_fn(v)
    # push into numpy
    m = np.empty((4,4))
    for i,j in np.ndindex(4,4):
        m[i,j] = v.GetElement(i,j)

    return m


def transform(M, v):
    v_ = np.dot(v, M[0:3,0:3])
    return v_ + M[0:3,-1]

def est_fn(u, b):
    est_unique = direct(u,b)
    # indices to expand unqiue coefficients to full tensor
    inds = np.array([[0, 1, 2],[1, 3, 4],[2, 4, 5]])
    def est_full(s):
        d = est_unique(s) # get unique coefficients
        return d[inds]    # convert to full tensor
    return est_full

def step(p, S, est, param):
    v = param['voxel']
    # unpack
    x,m_,fa = p[0],p[1],p[2]

    # move
    D = est(interp3signal(S,x))
    U,s,_ = np.linalg.svd(D)
    m = U[:,0]
    if np.dot(m_,m) < 0 : m = -m # align
    dx = m / v
    x = x + param['dt'] * dx[::-1]  # HACK volume dimensions are reversed
    fa = tensor2fa(D)

    return x,m,fa


def tensor2fa(T):
    T_ = np.trace(T)/3
    I = np.eye(3,3)
    return np.sqrt(1.5) * np.linalg.norm(T - T_*I,2) / np.linalg.norm(T,2)


def curve_radius(ff):
   if len(ff) < 3:
       return 1

   a,b,c = ff[-1],ff[-2],ff[-3]

   v1,v2 = b-c,a-b

   u1 = v1/np.linalg.norm(v1)
   u2 = v2/np.linalg.norm(v2)

   curv = np.linalg.norm(2*(u2-u1)/(np.linalg.norm(v2)+np.linalg.norm(v1)))
   if np.isnan(curv):
       return 1

   return 1/curv



def follow(S,u,b,mask,fiber,param):
    # initialize tract and estimator
    ff = [np.array(fiber[0])]
    est = est_fn(u,b)

    # main loop
    ct = 0
    while True :
        # estimate
        fiber = step(fiber, S, est, param)

        # unpack
        x,fa = fiber[0],fiber[2]

        # terminate if off brain or in CSF
        is_brain = interp3scalar(mask,x) > .1
        is_csf = fa < param['FA_min']
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

    #pp = [np.array([27,69,72])]

    print 'initial seed upper limit: %d' % (2*len(pp))
    # indices -> signals -> tensors
    ss = map(lambda p : interp3signal(S,p), pp)
    dd = map(direct(u,b), ss)

    # filter out only high-FA tensors
    mmll = map(unpack_tensor, dd)
    ff = map(lambda ml : l2fa(*ml[1]), mmll)
    ppmmff = map(lambda p,ml,fa : (p,ml[0],fa),  pp, mmll, ff)
    fa_min = param['FA_min']
    pri = filter(lambda pmlf : pmlf[2] > fa_min, ppmmff)

    # add in all opposite directions
    pri.extend(map(lambda pmf : (pmf[0],-pmf[1],pmf[2]), pri))

    return pri


def l2fa(a,b):
    return abs(a-b)/np.sqrt(a*a + 2*b*b)


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
    s *= 1e6
    assert s[0] > s[1] and s[1] > s[2]

    m = U[:,0]
    l = s[0], (s[1]+s[2])/2    # average two minor eigenvalues
    return m,l


def interp3scalar(M, p):
    sigma = 0.5
    s = 0
    w_sum = 0
    nx,ny,nz = M.shape

    for xx in [-1, 0, 1]:
        x = round(p[0]) + xx
        if x < 0 or nx <= x : continue
        dx = (x - p[0])**2
        for yy in [-1, 0, 1]:
            y = round(p[1]) + yy
            if y < 0 or ny <= y : continue
            dy = (y - p[1])**2
            for zz in [-1, 0, 1]:
                z = round(p[2]) + zz
                if z < 0 or nz <= z : continue
                dz = (z - p[2])**2
                w = np.exp( -(dx + dy + dz) / sigma )
                s     += w * M[x,y,z]
                w_sum += w

    return s / w_sum


def interp3signal(S, p):
    sigma = 0.5
    s = 0
    w_sum = 0
    nx,ny,nz,_ = S.shape

    for xx in [-1, 0, 1]:   # OPT: ndindex
        x = round(p[0]) + xx
        if x < 0 or nx <= x : continue
        dx = (x - p[0])**2
        for yy in [-1, 0, 1]:
            y = round(p[1]) + yy
            if y < 0 or ny <= y : continue
            dy = (y - p[1])**2
            for zz in [-1, 0, 1]:
                z = round(p[2]) + zz
                if z < 0 or nz <= z : continue
                dz = (z - p[2])**2
                w = np.exp( -(dx + dy + dz) / sigma )
                s     += w * S[x,y,z,:]
                w_sum += w

    s = np.hstack((s,s)) # replicate signal
    return s / (2*w_sum)
