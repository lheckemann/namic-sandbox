import sys
sys.path.insert(0, '/home/stefalie/proj/numpy-build/lib64/python2.6/site-packages')
sys.path.insert(0, '/home/stefalie/proj/weave-build/lib/python2.6/site-packages')
#sys.path.insert(0, '/projects/schiz/software/LabPython/lib/python2.6/site-packages')
#sys.path.insert(0, '/projects/lmi/people/stefalie/slicer/svn/Slicer3-lib/python-build/lib/python2.6/site-packages')

import numpy as np
#from scipy.weave import ext_tools as et
from weave import ext_tools as et


def add_model_1tensor_f(mod):
    # example data for type definitions
    X = np.empty((5,1), dtype='float64', order='F')
    m = X.shape[1]

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
        X += 5;
    }
    """
    fn = et.ext_function('c_model_1tensor_f', code, ['X', 'm'])
    mod.add_function(fn)


def add_model_1tensor_full_f(mod):
    X = np.empty((6,1), dtype='float64', order='F')
    m = X.shape[1]

    code = """
    for (int i = 0; i < m; ++i) {
        // lambda (clamped)
        X[3] = max(X[3], 100);
        X[4] = max(X[4], 100);
        X[5] = max(X[5], 100);

        // prepare for next
        X += 6;
    }
    """
    fn = et.ext_function('c_model_1tensor_full_f', code, ['X', 'm'])
    mod.add_function(fn)


def add_model_1tensor_h(mod):
    # typedefs
    X = np.empty((5,1), dtype='float64', order='F')
    u = np.empty((102,3), dtype='float64', order='F')
    b = 900.0

    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n,m), order='F')

    code = """
    for (int i = 0; i < m; ++i) {
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
        X += 5;
    }
    """

    fn = et.ext_function('c_model_1tensor_h', code, ['s', 'X', 'u', 'b', 'n', 'm'])
    mod.add_function(fn)


# Computes the signal from a given state.
def add_model_1tensor_full_h(mod):
    # typedefs
    X = np.empty((6, 1), dtype='float64', order='F')
    u = np.empty((102, 3), dtype='float64', order='F')
    b = 900.0

    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n, m), order='F')

    code = """
    for (int i = 0; i < m; ++i) {
        // unpack
        double theta = X[0], phi = X[1], psi = X[2];
        double l11 = X[3], l12 = X[4], l13 = X[5];

        // ensure: lambda >= L
        double L = 100;
        if (l11 < L)   l11 = L;
        if (l12 < L)   l12 = L;
        if (l13 < L)   l13 = L;

        // calculate diffusion matrix
        mat_t D = diffusion_euler(theta, phi, psi, l11, l12, l13);

        // reconstruct signal
        for (int i = 0; i < n; ++i, ++s) {
            vec_t u_ = make_vec(u[3 * i], u[3 * i + 1], u[3 * i + 2]);
            *s = exp(-b * dot(u_, D * u_));
        }

        // prepare for next
        X += 6;
    }
    """

    fn = et.ext_function('c_model_1tensor_full_h', code, ['s', 'X', 'u', 'b', 'n', 'm'])
    mod.add_function(fn)


def add_state2tensor1(mod):
    # example data for type definitions
    X = np.empty((5,1), dtype='float64', order='F')
    y = np.empty(3)

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

    fn = et.ext_function('c_state2tensor1', code, ['X', 'y', 'm', 'l1', 'l2'])
    mod.add_function(fn)


# Get's main direction and the Eigenvalues from the state.
def add_state2tensor1_full(mod):
    X = np.empty((6,1), dtype='float64', order='F')
    y = np.empty(3)

    code = """
    double theta = X[0], phi = X[1], psi = X[2];
    mat_t Q = rotation(theta, phi, psi);

    m[0] = Q._[0];
    m[1] = Q._[3];
    m[2] = Q._[6];

    // Flip orientation if necessary.
    //if (m[0] * y[0] + m[1] * y[1] + m[2] * y[2] < 0) {
    //    for (int i = 0; i < 3; ++i)
    //        m[i] = -m[i];
    //}

    // Clamp lambdas.
    *l1 = max(X[3], 100);
    *l2 = max(X[4], 100);
    *l3 = max(X[5], 100);
    """

    m = np.empty((3,1))
    l1 = np.empty(1);
    l2 = np.empty(1);
    l3 = np.empty(1);

    fn = et.ext_function('c_state2tensor1_full', code, ['X', 'y', 'm', 'l1', 'l2', 'l3'])
    mod.add_function(fn)


def add_model_2tensor_f(mod):
    X = np.empty((10, 1), dtype='float64', order='F')
    m = X.shape[1]

    code = """
    for (int i = 0; i < m; ++i) {
        double *o1 = X, *o2 = X + 5;

        // unpack and normalize orientations
        vec_t m1 = make_vec(o1[0], o1[1], o1[2]); m1 = m1 / norm(m1);
        vec_t m2 = make_vec(o2[0], o2[1], o2[2]); m2 = m2 / norm(m2);
        double l11 = o1[3], l12 = o1[4];
        double l21 = o2[3], l22 = o2[4];
    
        // ensure: lambda >= L
        double L = 100;
        if (l11 < L)   l11 = L;
        if (l12 < L)   l12 = L;
        if (l21 < L)   l21 = L;
        if (l22 < L)   l22 = L;

        // write back
        vec2mem(m1, o1);  vec2mem(m2, o2);
        o1[3] = l11; o2[3] = l21;
        o1[4] = l12; o2[4] = l22;

        // Prepare for next.
        X += 10;
    }
    """

    fn = et.ext_function('c_model_2tensor_f', code, ['X', 'm'])
    mod.add_function(fn)


def add_model_2tensor_full_f(mod):
    X = np.empty((12, 1), dtype='float64', order='F')
    m = X.shape[1]

    code = """
    for (int i = 0; i < m; ++i) {
        // Clamp lambdas.
        double *o = X;
        o[3] = max(o[3], 100);
        o[4] = max(o[4], 100);
        o[5] = max(o[5], 100);

        o[9] = max(o[9], 100);
        o[10] = max(o[10], 100);
        o[11] = max(o[11], 100);

        // Prepare for next.
        X += 12;
    }
    """

    fn = et.ext_function('c_model_2tensor_full_f', code, ['X', 'm'])
    mod.add_function(fn)


def add_model_2tensor_h(mod):
    # typedefs
    X = np.empty((10, 1), dtype='float64', order='F')
    u = np.empty((102, 3), dtype='float64', order='F')
    b = 900.0

    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n, m), order='F')

    code = """
    for (int i = 0; i < m; ++i) {
        double *o1 = X, *o2 = X + 5;
        // unpack and normalize orientations
        vec_t m1 = make_vec(o1[0], o1[1], o1[2]); m1 /= norm(m1);
        double l11 = o1[3], l12 = o1[4];
        vec_t m2 = make_vec(o2[0], o2[1], o2[2]); m2 /= norm(m2);
        double l21 = o2[3], l22 = o2[4];

        // ensure: lambda >= L
        double L = 100;
        if (l11 < L)   l11 = L;
        if (l12 < L)   l12 = L;
        if (l21 < L)   l21 = L;
        if (l22 < L)   l22 = L;

        // flip if necessary
        if (m1._[0] < 0)   m1 = -m1;
        if (m2._[0] < 0)   m2 = -m2;

        // calculate diffusion matrix
        mat_t D1 = diffusion(m1, l11, l12);
        mat_t D2 = diffusion(m2, l21, l22);

        // reconstruct signal
        for (int i = 0; i < n; ++i, ++s) {
            vec_t u_ = make_vec(u[3*i], u[3*i+1], u[3*i+2]);
            *s = (exp(-b*dot(u_,D1*u_)) + exp(-b*dot(u_,D2*u_)))/2;
        }

        // prepare for next
        X += 10;
    }
    """

    fn = et.ext_function('c_model_2tensor_h', code, ['s', 'X', 'u', 'b', 'n', 'm'])
    mod.add_function(fn)


def add_model_2tensor_full_h(mod):
    # typedefs
    X = np.empty((12, 1), dtype='float64', order='F')
    u = np.empty((102, 3), dtype='float64', order='F')
    b = 900.0

    n = u.shape[0]
    m = X.shape[1]
    s = np.empty((n, m), order='F')

    code = """
    for (int i = 0; i < m; ++i) {
        // Unpack the data.
        double *o1 = X, *o2 = X + 6;
        double theta1 = o1[0], phi1 = o1[1], psi1 = o1[2];
        double theta2 = o2[0], phi2 = o2[1], psi2 = o2[2];
        double l11 = o1[3], l12 = o1[4], l13 = o1[5];
        double l21 = o2[3], l22 = o2[4], l23 = o2[5];

        // Ensure: lambda >= L.
        double L = 100;
        if (l11 < L)   l11 = L;
        if (l12 < L)   l12 = L;
        if (l13 < L)   l13 = L;

        if (l21 < L)   l21 = L;
        if (l22 < L)   l22 = L;
        if (l23 < L)   l23 = L;

        // Calculate diffusion matrix.
        mat_t D1 = diffusion_euler(theta1, phi1, psi1, l11, l12, l13);
        mat_t D2 = diffusion_euler(theta2, phi2, psi2, l21, l22, l23);

        // Reconstruct signal.
        for (int i = 0; i < n; ++i, ++s) {
            vec_t u_ = make_vec(u[3 * i], u[3 * i + 1], u[3 * i + 2]);
            *s = (exp(-b * dot(u_, D1 * u_)) + exp(-b * dot(u_, D2 * u_))) / 2.0;
        }

        // Prepare for next.
        X += 12;
    }
    """
    fn = et.ext_function('c_model_2tensor_full_h', code, ['s', 'X', 'u', 'b', 'n', 'm'])
    mod.add_function(fn)


def add_state2tensor2(mod):
    # example data for type definitions
    X = np.empty((5,1), dtype='float64', order='F')
    y = np.empty(3)

    code = """
    #line 0 "state2tensor2"
    //-- first orientation
    // unpack
    double m1_norm = 1e-10;
    for (int i = 0; i < 3; ++i) {
      double x = m1[i] = X[i];
      m1_norm += x * x;
    }
    // normalize
    m1_norm = sqrt(m1_norm);
    for (int i = 0; i < 3; ++i)
      m1[i] /= m1_norm;

    // flip orientation?
    if (m1[0]*y[0] + m1[1]*y[1] + m1[2]*y[2] < 0) {
        for (int i = 0; i < 3; ++i)
            m1[i] = -m1[i];
    }

    // lambda (clamped)
    *l11 = max(X[3], 100);
    *l12 = max(X[4], 100);



    //-- second orientation
    X += 5;
    // unpack
    double m2_norm = 1e-10;
    for (int i = 0; i < 3; ++i) {
      double x = m2[i] = X[i];
      m2_norm += x * x;
    }
    // normalize
    m2_norm = sqrt(m2_norm);
    for (int i = 0; i < 3; ++i)
      m2[i] /= m2_norm;

    // flip orientation?
    if (m2[0]*y[0] + m2[1]*y[1] + m2[2]*y[2] < 0) {
        for (int i = 0; i < 3; ++i)
            m2[i] = -m2[i];
    }

    // lambda (clamped)
    *l21 = max(X[3], 100);
    *l22 = max(X[4], 100);
    """
    m1 = np.empty((3,1))
    m2 = np.empty((3,1))
    l11,l12 = np.empty(1),np.empty(1);
    l21,l22 = np.empty(1),np.empty(1);

    fn = et.ext_function('c_state2tensor2', code, ['X', 'y', 'm1', 'l11', 'l12', 'm2', 'l21', 'l22'])
    mod.add_function(fn)


def add_state2tensor2_full(mod):
    # example data for type definitions
    X = np.empty((6, 1), dtype='float64', order='F')
    y = np.empty(3)

    code = """
    // First orientation.
    double theta = X[0], phi = X[1], psi = X[2];
    mat_t Q = rotation(theta, phi, psi);

    m1[0] = Q._[0];
    m1[1] = Q._[3];
    m1[2] = Q._[6];

    // Flip orientation if necessary.
    if (m1[0] * y[0] + m1[1] * y[1] + m1[2] * y[2] < 0) {
        for (int i = 0; i < 3; ++i)
            m1[i] = -m1[i];
    }

    // Clamp lambdas.
    *l11 = max(X[3], 100);
    *l12 = max(X[4], 100);
    *l13 = max(X[5], 100);

    // Second Orientation.
    X += 6;
    theta = X[0];
    phi = X[1];
    psi = X[2];
    Q = rotation(theta, phi, psi);

    m2[0] = Q._[0];
    m2[1] = Q._[3];
    m2[2] = Q._[6];

    // Flip orientation if necessary.
    if (m2[0] * y[0] + m2[1] * y[1] + m2[2] * y[2] < 0) {
        for (int i = 0; i < 3; ++i)
            m2[i] = -m2[i];
    }

    // Clamp lambdas.
    *l21 = max(X[3], 100);
    *l22 = max(X[4], 100);
    *l23 = max(X[5], 100);
    """

    m1 = np.empty((3,1))
    m2 = np.empty((3,1))
    l11, l12, l13 = np.empty(1), np.empty(1), np.empty(1);
    l21, l22, l23 = np.empty(1), np.empty(1), np.empty(1);

    fn = et.ext_function('c_state2tensor2_full', code, ['X', 'y', 'm1', 'l11', 'l12', 'l13', 'm2', 'l21', 'l22', 'l23'])
    mod.add_function(fn)


def add_s2ga(mod):
    s = np.empty((102,1), dtype='float64')
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
    fn = et.ext_function('c_s2ga', code, ['s', 'n'])
    mod.add_function(fn)


def add_interp3signal(mod):
    S = np.empty((100,100,100,52), dtype='float32', order='F')
    p = np.empty(3)
    v = np.empty(3)
    sigma = 0.

    code = """
           #line 0 "interp3signal"
           double w_sum = 1e-16;
           double px = p[0], py = p[1], pz = p[2];
           double vx = v[0], vy = v[1], vz = v[2];

           for (int i = 0; i < 2*n; ++i)
               s[i] = 0;

           for (int zz = -1; zz <= 1; ++zz) {
               int z = round(pz) + zz;
               if (z < 0 || nz <= z) continue;
               double dz = (z - pz)*vz, dzz = dz*dz;

               for (int yy = -1; yy <= 1; ++yy) {
                   int y = round(py) + yy;
                   if (y < 0 || ny <= y) continue;
                   double dy = (y - py)*vy, dyy = dy*dy;

                   for (int xx = -1; xx <= 1; ++xx) {
                       int x = round(px) + xx;
                       if (x < 0 || nx <= x) continue;
                       double dx = (x - px)*vx, dxx = dx*dx;
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
    s = np.empty((2*n,), dtype='float32')  # preallocate output (doubled)
    fn = et.ext_function('c_interp3signal', code, ['s','S', 'p', 'v', 'sigma', 'nx','ny','nz','n'])
    mod.add_function(fn)


def add_interp3scalar(mod):
    M = np.empty((100,100,100), dtype='uint16', order='F')
    p = np.empty(3)
    v = np.empty(3)
    sigma = 0.

    code = """
           #line 0 "interp3scalar"
           double s = 0, w_sum = 1e-16;
           double px = p[0], py = p[1], pz = p[2];
           double vx = v[0], vy = v[1], vz = v[2];

           for (int zz = -1; zz <= 1; zz++) {
               int z = round(pz) + zz;
               if (z < 0 || nz <= z) continue;
               double dz = (z - pz)*vz, dzz = dz*dz;

               for (int yy = -1; yy <= 1; yy++) {
                   int y = round(py) + yy;
                   if (y < 0 || ny <= y) continue;
                   double dy = (y - py)*vy, dyy = dy*dy;

                   for (int xx = -1; xx <= 1; xx++) {
                       int x = round(px) + xx;
                       if (x < 0 || nx <= x) continue;
                       double dx = (x - px)*vx, dxx = dx*dx;

                       float w = exp( -(dxx + dyy + dzz)/sigma );
                       if (M[nz*ny*x + nz*y + z]) {
                         s += w;
                       }
                       //float d = M[nz*ny*x + nz*y + z];
                       //s     += w * d;
                       w_sum += w;
                   }
               }
           }
           return_val = s / w_sum;
           """
    nx,ny,nz = M.shape

    fn = et.ext_function('c_interp3scalar', code, ['M', 'p', 'v', 'sigma', 'nx','ny','nz'])
    mod.add_function(fn)


# helper routines for matrix/vector manipulation
matrix_code = """
    #define max(x,y)  ((x) > (y) ? (x) : (y))

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

    inline mat_t diffusion(vec_t m, double l1, double l2)
    {
        double x = m._[0], y = m._[1], z = m._[2];
        mat_t R = make_mat(x,  y,            z          ,
                           y,  y*y/(1+x)-1,  y*z/(1+x)  ,
                           z,  y*z/(1+x),    z*z/(1+x)-1);
        return R * diag(l1,l2,l2) * t(R) * 1e-6;
    }

    inline mat_t rotation(double theta, double phi, double psi)
    {
        double c_th = cos(theta);
        double s_th = sin(theta);
        double c_ph = cos(phi);
        double s_ph = sin(phi);
        double c_ps = cos(psi);
        double s_ps = sin(psi);

        double q11 = c_th * c_ph * c_ps - s_ph * s_ps;
        double q21 = c_th * c_ps * s_ph + c_ph * s_ps;
        double q31 = -c_ps * s_th;
        double q12 = -c_ps * s_ph - c_th * c_ph * s_ps;
        double q22 = c_ph * c_ps - c_th * s_ph * s_ps;
        double q32 = s_th * s_ps;
        double q13 = c_ph * s_th;
        double q23 = s_th * s_ph;
        double q33 = c_th;

        mat_t Q = make_mat(q11, q12, q13,
                           q21, q22, q23,
                           q31, q32, q33);
        return Q;
    }

    // Calculates a rotation matrix from euler angles.
    inline mat_t diffusion_euler(double theta, double phi, double psi,
                                 double l1, double l2, double l3)
    {
        mat_t Q = rotation(theta, phi, psi);
        return Q * diag(l1,l2,l3) * t(Q) * 1e-6;
    }
    """



def build():
    mod = et.ext_module('filtered_ext')

    mod.customize.add_support_code(matrix_code)

    # single-tensor
    add_model_1tensor_f(mod)
    add_model_1tensor_full_f(mod)
    add_model_1tensor_h(mod)
    add_model_1tensor_full_h(mod)
    add_state2tensor1(mod)
    add_state2tensor1_full(mod)

    # two-tensor
    add_model_2tensor_f(mod)
    add_model_2tensor_full_f(mod)
    add_model_2tensor_h(mod)
    add_model_2tensor_full_h(mod)
    add_state2tensor2(mod)
    add_state2tensor2_full(mod)

    # utility
    add_interp3signal(mod)
    add_interp3scalar(mod)
    add_s2ga(mod)

    mod.compile()


build()
import filtered_ext
