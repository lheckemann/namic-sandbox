# old python versions of various utilities

def s2ga(s):
    mu    = s.mean(axis=0)
    mu_sq = np.square(s).mean(axis=0)
    ga    = np.sqrt(mu_sq - np.square(mu)) / np.sqrt(mu_sq)
    return ga


def interp3scalar(M, p):
    sigma = 0.5
    s = 0
    w_sum = 1e-10
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
    w_sum = 1e-10
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


def state2tensor(X, y=np.zeros(3)):
    assert X.ndim == 1 or X.shape[1] == 1  # only handle single
    # unpack and normalize
    m,l1,l2 = X[0:3],X[3],X[4]
    m = m / np.linalg.norm(m)
    if l1 < 100 : l1 = 100
    if l2 < 100 : l2 = 100
    # flip if necessary
    if dot(m,y) < 0 : m = -m
    return m,(l1,l2)


def _model_1tensor_f(X):
    for i in range(0,X.shape[1]):
        m,(l1,l2) = _state2tensor(X[:,i])
        X[0:3,i] = m
        X[3,i] = l1
        X[4,i] = l2
    return X

# construct diffusion matrix from principal direction and major/minor eigenvalues
def diffusion(m,(l1,l2)):
    # flip if necessary
    if m[0] < 0 : m = -m
    x,y,z = m[0],m[1],m[2]
    R = np.array([[x,  y,            z          ],
                  [y,  y*y/(1+x)-1,  y*z/(1+x)  ],
                  [z,  y*z/(1+x),    z*z/(1+x)-1]])
    L = np.zeros((3,3))   # HACK
    L[0,0] = l1
    L[1,1] = l2
    L[2,2] = l2
    R = np.mat(R) # HACK
    return R * L * R.T * 1e-6

def _model_1tensor_h(X,u,b):
    n = u.shape[0]
    m = X.shape[1]
    S = np.empty((n,m))
    for i in range(0,m):
        # unpack and normalize
        m,l = state2tensor(X[:,i])
        # construction diffusion matrix
        D = diffusion(m,l)
        for j in range(0,n):
            u_ = np.mat(u[j,:])   # HACK
            S[j,i] = np.exp(-b * u_ * (D * u_.T))
    return S



def _state2tensor(X, y=np.zeros(3)):
    assert X.ndim == 1 or X.shape[1] == 1  # only handle single
    # unpack and normalize
    m,l1,l2 = X[0:3],X[3],X[4]
    m = m / norm(m)
    if l1 < 100 : l1 = 100
    if l2 < 100 : l2 = 100
    # flip if necessary
    if dot(m,y) < 0 : m = -m
    return m,(l1,l2)

def s2ga(s):
    mu    = s.mean(axis=0)
    mu_sq = np.square(s).mean(axis=0)
    ga    = np.sqrt(mu_sq - np.square(mu)) / np.sqrt(mu_sq)
    return ga




def compare_state2tensor(X, m):
    import time

    print _state2tensor(X)
    print state2tensor(X)

    t1 = time.time()
    for i in range(m):
        _state2tensor(X)
    t2 = time.time()
    py = t2 - t1
    print 'python speed:', py

    t1 = time.time()
    for i in range(m):
        state2tensor(X)
    t2 = time.time()
    c = t2-t1
    print 'c speed:', c

    print 'py/c  ', py / c
