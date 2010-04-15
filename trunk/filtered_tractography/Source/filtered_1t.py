XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Tractography</category>
  <title>Filtered: One Tensor</title>
  <description>Filtered tractography with single-tensor model.</description>

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
from scipy import weave

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
    mf  = vtk2mat(dwi_node.GetMeasurementFrameMatrix, slicer)
    r2i = vtk2mat(dwi_node.GetRASToIJKMatrix, slicer)
    i2r = vtk2mat(dwi_node.GetIJKToRASMatrix, slicer)

    # tractography...
    param = dict({'FA_min': .1,  # FA stopping threshold
                  'GA_min': .1,  # generalized anisotropy stopping threshold
                  'dt': .3,     # forward Euler step size (path integration)
                  'max_len': 250, # stop if fiber gets this long
                  'min_radius': .87,  # stop if fiber curves this much
                  'seeds': 5, # how many seeds to spawn in each ROI voxel
                  'voxel': np.mat('1.66; 1.66; 1.70'), # voxel size (check your .nhdr file)
                  # Kalman filter parameters
                  'Qm': .0015,  # injected angular noise (probably leave untouched)
                  'Ql': 25,    # injected eigenvalue noise (probably leave untouched)
                  'Rs': .020,  # dependent on latent noise in your data (likely change this)
                  'P0': np.eye(5,5) / 100,}) # initial covariance (likely change this)
    ff = init(S, seeds, u, b, param)
    for i in range(0,len(ff)):
        print '[%3.0f%%] (%7d - %7d)' % (100.0*i/len(ff), i, len(ff))
        ff[i] = follow(S,u,b,mask,ff[i],param)

    # build polydata
    pts = slicer.vtkPoints()
    lines = slicer.vtkCellArray()
    cell_id = 0
    for i in range(0,len(ff)):
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
    # unpack
    x,X,P = p[0],p[1],p[2]
    # move
    X,P = est(X,P,interp3signal(S,x))
    m,(l1,l2) = state2tensor(X)
    dx = m / param['voxel']
    x = x + param['dt'] * dx[::-1]  # HACK volume dimensions are reversed
    # repack
    return x,X,P


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
    f_fn,h_fn = model_1tensor(u,b)
    Qm = param['Qm']*np.eye(3,3)
    Ql = param['Ql']*np.eye(2,2)
    Rs = param['Rs']*np.eye(u.shape[0],u.shape[0])
    Q = np.zeros((5,5))
    Q[0:3,0:3],Q[3:5,3:5] = Qm,Ql
    est = filter_ukf(f_fn,h_fn,Q,Rs)

    # main loop
    ct = 0
    while True :
        # estimate
        fiber = step(fiber, S, est, param)

        # unpack
        x,X = fiber[0],fiber[1]
        _,l = state2tensor(X)

        # terminate if off brain or in CSF
        is_brain = interp3scalar(mask,x) > .1
        is_csf = l2fa(l) < param['FA_min']
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

    #pp = [np.array((27,69,72))] # HACK

    print 'initial seed upper limit: %d' % (2*len(pp))
    # indices -> signals -> tensors
    ss = map(lambda p : interp3signal(S,p), pp)
    dd = map(direct(u,b), ss)

    # {position, direction, lambda, covariance}
    mmll = map(unpack_tensor, dd)
    P0 = np.mat(param['P0'])
    ff = map(lambda p,ml : (p,ml[0],ml[1],P0), pp, mmll)

    # filter out only high-FA tensors
    fa_min = param['FA_min']
    ff = filter(lambda pml : l2fa(pml[2]) > fa_min, ff)

    # add in all opposite directions
    ff.extend(map(lambda pmlP : (pmlP[0],-pmlP[1],pmlP[2],pmlP[3]), ff))

    # convert to column vectors
    ff = map(lambda x : (np.mat(x[0]).T, np.mat(np.hstack((x[1],x[2]))).reshape(5,1), x[3]), ff)

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
        K,_,_,_ = np.linalg.lstsq(Pyy, Pxy.T)
        P = P - K.T * Pyy * K
        z = z.reshape(Pyy.shape[0],1)
        x = x_hat + K.T * (z - Y_hat)

        return x,P

    return filter


def model_1tensor(u,b):
    f_fn = model_1tensor_f   # identity, but fix up state
    h_fn = lambda X : model_1tensor_h(X,u,b)
    return f_fn,h_fn

def model_1tensor_f(X):
    k,m = X.shape
    code = """
    for (int i = 0; i < m; ++i) {
        // normalize
        double m_norm = 1e-16;
        for (int i = 0; i < 3; ++i) {
          double x = X[i];
          m_norm += x * x;
        }
        m_norm = sqrt(m_norm);
        for (int i = 0; i < 3; ++i)
          X[i] /= m_norm;

        // lambda (clamped)
        X[3] = max(X[3], 100);
        X[4] = max(X[4], 100);

        // prepare for next
        X += k;
    }
    """
    X = np.copy(X)
    weave.inline(code, ['X', 'k', 'm'],
                 support_code = "#define max(x,y)  ((x) > (y) ? (x) : (y))")
    return X



def state2tensor(X, y=np.zeros(3)):
    assert X.ndim == 1 or X.shape[1] == 1  # only handle single
    code = """
    // unpack
    double m_norm = 1e-10;
    for (int i = 0; i < 3; ++i) {
      double x = m[i] = X[i];
      m_norm += x * x;
    }
    // normalize
    m_norm = sqrt(m_norm);
    for (int i = 0; i < 3; ++i)
      m[i] /= m_norm;

    // flip orientation?
    if (m[0]*y[0] + m[1]*y[1] + m[2]*y[2] < 0) {
        for (int i = 0; i < 3; ++i)
            m[i] = -m[i];
    }

    // lambda (clamped)
    *l1 = max(X[3], 100);
    *l2 = max(X[4], 100);
    """
    m = np.empty((3,1))
    l1 = np.empty(1);
    l2 = np.empty(1);
    weave.inline(code, ['X', 'y', 'm', 'l1', 'l2'],
                 support_code = "#define max(x,y)  ((x) > (y) ? (x) : (y))");
    return m,(l1,l2)

def l2fa((a,b)):
    return abs(a-b)/np.sqrt(a*a + 2*b*b)

def dot(a,b):
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]













def model_1tensor_h(X,u,b):
    n = u.shape[0]
    k,m = X.shape
    s = np.empty((n,m))

    help = """
    static mat_t diffusion(vec_t m, double l1, double l2)
    {
        double x = m._[0], y = m._[1], z = m._[2];
        mat_t R = make_mat(x,  y,            z          ,
                           y,  y*y/(1+x)-1,  y*z/(1+x)  ,
                           z,  y*z/(1+x),    z*z/(1+x)-1);
        return R * diag(l1,l2,l2) * t(R) * 1e-6;
    }
    """

    code = """
    for (int i = 0; i < m; i++) {
        // unpack and normalize orientations
        vec_t m1 = make_vec(X[0], X[1], X[2]); m1 /= norm(m1);
        double l11 = X[3], l12 = X[4];

        // ensure: lambda >= L
        double L = 100;
        if (l11 < L)   l11 = L;
        if (l12 < L)   l12 = L;

        // flip if necessary
        if (m1._[0] < 0)   m1 = -m1;

        // calculate diffusion matrix
        mat_t D = diffusion(m1, l11, l12);

        // reconstruct signal
        for (int i = 0; i < n; ++i, ++s) {
            vec_t u_ = make_vec(u[3*i], u[3*i+1], u[3*i+2]);
            *s = exp(-b*dot(u_,D*u_));
        }

        // prepare for next
        X += k;
    }
    """
    weave.inline(code, ['s', 'X', 'u', 'b', 'n', 'k', 'm'],
                 support_code = matrix_code + help);
    return s



def s2ga(s):
    n = s.shape[0]
    code = """
           #line 0 "s2ga"
           double mu = 0, mu_sq = 0;
           for (int i = 0; i < n; ++i, ++s) {
               mu    += *s;
               mu_sq += *s * *s;
           }
           mu    /= n;
           mu_sq /= n;
           double ga = sqrt(mu_sq - mu*mu) / sqrt(mu_sq);
           return_val = ga;
           """
    return weave.inline(code, ['s', 'n'])


def interp3signal(S, p):
    code = """
           #line 0 "interp3signal"
           double w_sum = 1e-10, sigma = 0.5;
           float px = p[0], py = p[1], pz = p[2];

           for (int i = 0; i < 2*n; ++i)
               s[i] = 0;

           for (int zz = -1; zz <= 1; ++zz) {
               int z = round(pz) + zz;
               if (z < 0 || nz <= z) continue;
               int dz = z - pz, dzz = dz*dz;

               for (int yy = -1; yy <= 1; ++yy) {
                   int y = round(py) + yy;
                   if (y < 0 || ny <= y) continue;
                   int dy = y - py, dyy = dy*dy;

                   for (int xx = -1; xx <= 1; ++xx) {
                       int x = round(px) + xx;
                       if (x < 0 || nx <= x) continue;
                       int dx = x - px, dxx = dx*dx;

                       //dx *= v[0]; dy *= v[1]; dz *= v[2];
                       double w = exp( -(dxx + dyy + dzz)/sigma );

                       for (int i = 0; i < n; ++i)
                           s[i] += w * S[(nz*ny*x + nz*y + z)*n + i];

                       w_sum += w;
                   }
               }
           }

           w_sum *= 2; // double each occurance
           for (int i = 0; i < n; ++i) {
               s[i  ] /= w_sum;
               s[i+n]  = s[i]; // push into second spot
           }

           """
    nx,ny,nz,n = S.shape
    s = np.zeros((2*n,), dtype='float32')  # preallocate output (doubled)
    weave.inline(code, ['s','S', 'p', 'nx','ny','nz','n'])
    return s


def interp3scalar(M, p):
    code = """
           #line 0 "interp3scalar"
           float s = 0, w_sum = 1e-10, sigma = 0.5;
           float px = p[0], py = p[1], pz = p[2];

           for (int zz = -1; zz <= 1; zz++) {
               int z = round(pz) + zz;
               if (z < 0 || nz <= z) continue;
               float dz = z - pz, dzz = dz*dz;

               for (int yy = -1; yy <= 1; yy++) {
                   int y = round(py) + yy;
                   if (y < 0 || ny <= y) continue;
                   float dy = y - py, dyy = dy*dy;

                   for (int xx = -1; xx <= 1; xx++) {
                       int x = round(px) + xx;
                       if (x < 0 || nx <= x) continue;
                       float dx = x - px, dxx = dx*dx;
                       //dx *= v[0]; dy *= v[1]; dz *= v[2];
                       float w = exp( -(dxx + dyy + dzz)/sigma );
                       float d = M[nz*ny*x + nz*y + z];
                       s     += w * d;
                       w_sum += w;
                   }
               }
           }
           return_val = s / w_sum;
           """
    nx,ny,nz = M.shape
    return weave.inline(code, ['M', 'p', 'nx','ny','nz'])



# helper routines for matrix/vector manipulation
matrix_code = """
    typedef struct { double _[3]; } vec_t;

    inline vec_t make_vec(double x, double y, double z)
    {
        vec_t m = { {x,y,z} };
        return m;
    }
    inline vec_t make_vec(double th, double phi)
    {
        double cosphi = cos(phi);
        double x = cosphi * cos(th), y = cosphi * sin(th), z = sin(phi);
        return make_vec(x,y,z);
    }
    inline vec_t make_vec_s(double *sph)
    {
        double th = sph[0], phi = sph[1];
        return make_vec(th,phi);
    }
    inline vec_t make_vec_c(double *m)
    {
        return make_vec(m[0],m[1],m[2]);
    }
    inline void vec2sph(vec_t m, double *sph)
    {
        double x = m._[0], y = m._[1], z = m._[2];
        sph[0] = atan2(y, x);
        sph[1] = atan2(z, hypot(x, y));
    }
    inline void vec2mem(vec_t v, double *m)
    {
        for (int i = 0; i < 3; i++)
            m[i] = v._[i];
    }


    // negation
    inline vec_t operator-(vec_t a)
    {
        return make_vec(-a._[0], -a._[1], -a._[2]);
    }

    // addition
    inline vec_t operator+(vec_t a, double b)
    {
        return make_vec(a._[0]+b, a._[1]+b, a._[2]+b);
    }
    inline vec_t operator+(double a, vec_t b) { return b + a; }
    inline void operator+=(vec_t &a, double b)
    {
        a._[0] += b;
        a._[1] += b;
        a._[2] += b;
    }

    // subtraction
    inline vec_t operator-(vec_t a, double b)
    {
        return make_vec(a._[0]-b, a._[1]-b, a._[2]-b);
    }
    inline vec_t operator-(double a, vec_t b)
    {
        return make_vec(a-b._[0], a-b._[1], a-b._[2]);
    }
    inline void operator-=(vec_t &a, double b)
    {
        a._[0] -= b;
        a._[1] -= b;
        a._[2] -= b;
    }

    // multiplication
    inline vec_t operator*(vec_t a, double b)
    {
        return make_vec(a._[0]*b, a._[1]*b, a._[2]*b);
    }
    inline vec_t operator*(double a, vec_t b) { return b * a; }
    inline void operator*=(vec_t &a, double b)
    {
        a._[0] *= b;
        a._[1] *= b;
        a._[2] *= b;
    }

    // division
    inline vec_t operator/(vec_t a, double b)
    {
        return make_vec(a._[0]/b, a._[1]/b, a._[2]/b);
    }
    inline vec_t operator/(double a, vec_t b)
    {
        return make_vec(a/b._[0], a/b._[1], a/b._[2]);
    }
    inline void operator/=(vec_t &a, double b)
    {
        a._[0] /= b;
        a._[1] /= b;
        a._[2] /= b;
    }


    inline double norm(vec_t a)
    {
        return sqrt(a._[0]*a._[0] + a._[1]*a._[1] + a._[2]*a._[2]);
    }


    inline double dot(vec_t a, vec_t b)
    {
        return a._[0]*b._[0] + a._[1]*b._[1] + a._[2]*b._[2];
    }

    typedef struct { double _[9]; } mat_t;

    inline mat_t make_mat(double a, double b, double c,
                          double d, double e, double f,
                          double g, double h, double i)
    {
        mat_t m = { {a,b,c, d,e,f, g,h,i} };
        return m;
    }
    inline mat_t diag(double a, double b, double c)
    {
        mat_t m = { {a,0,0, 0,b,0, 0,0,c} };
        return m;
    }
    inline mat_t t(mat_t m)
    {
        return make_mat(m._[0], m._[3], m._[6],
                        m._[1], m._[4], m._[7],
                        m._[2], m._[5], m._[8]);
    }

    // matrix-scalar
    inline mat_t operator*(mat_t a, double b)
    {
        return make_mat(a._[0]*b,  a._[1]*b, a._[2]*b,
                        a._[3]*b,  a._[4]*b, a._[5]*b,
                        a._[6]*b,  a._[7]*b, a._[8]*b);
    }
    inline mat_t operator*(double a, mat_t b)
    {
        return b * a;
    }

    // matrix-vector
    inline vec_t operator*(mat_t a, vec_t b)
    {
        return make_vec(a._[0]*b._[0] + a._[1]*b._[1] + a._[2]*b._[2],
                        a._[3]*b._[0] + a._[4]*b._[1] + a._[5]*b._[2],
                        a._[6]*b._[0] + a._[7]*b._[1] + a._[8]*b._[2]);
    }


    // matrix-matrix
    inline mat_t operator*(mat_t a, mat_t b)
    {
        return make_mat(a._[0]*b._[0] + a._[1]*b._[3] + a._[2]*b._[6],
                        a._[0]*b._[1] + a._[1]*b._[4] + a._[2]*b._[7],
                        a._[0]*b._[2] + a._[1]*b._[5] + a._[2]*b._[8],

                        a._[3]*b._[0] + a._[4]*b._[3] + a._[5]*b._[6],
                        a._[3]*b._[1] + a._[4]*b._[4] + a._[5]*b._[7],
                        a._[3]*b._[2] + a._[4]*b._[5] + a._[5]*b._[8],

                        a._[6]*b._[0] + a._[7]*b._[3] + a._[8]*b._[6],
                        a._[6]*b._[1] + a._[7]*b._[4] + a._[8]*b._[7],
                        a._[6]*b._[2] + a._[7]*b._[5] + a._[8]*b._[8]);
    }

    // determinant
    inline double det(mat_t M)
    {
        return M._[0]*(M._[4]*M._[8] - M._[5]*M._[7])
            -  M._[1]*(M._[3]*M._[8] - M._[5]*M._[6])
            +  M._[2]*(M._[3]*M._[7] - M._[4]*M._[6]);
    }

    // conjugate transpose
    inline mat_t ct(mat_t M)
    {
        return make_mat( M._[0], -M._[3],  M._[6],
                        -M._[1],  M._[4], -M._[7],
                         M._[2], -M._[5],  M._[8]);
    }

    // invert
    inline mat_t inv(mat_t M)
    {
        return (1 / det(M)) * ct(M);
    }
    """
