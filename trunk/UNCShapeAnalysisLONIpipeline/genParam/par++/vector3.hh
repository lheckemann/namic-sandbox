static const char SccsId_vector3[] = "@(#)vector3.hh  3.5 12 Apr 1994";

double  dotproduct3(const double *, const double *);
double  det3(const double *, const double *, const double *);
void  normalize(const int nvectors, const int dimension, double *x);
double  spher_area4(const double *x, const int corner[4], double angle[4]);
void  spher_step(double step, double *src, double *vec, int, double *dest);
void  copy_vector(double *dest, const double *src, const int length);

inline double sqr(double x) { return x*x;} // sqr is no longer in math.h
