static const char SccsId[] = "@(#)vector3.cc  3.8 23 Jul 1994";

#include "vector3.hh"  
#include <math.h>


double dotproduct3(const double *a, const double *b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}



double det3(const double *a, const double *b, const double *c)
{
  return a[0]*(b[1]*c[2] - b[2]*c[1]) +
   a[1]*(b[2]*c[0] - b[0]*c[2]) +
   a[2]*(b[0]*c[1] - b[1]*c[0]);
}



void normalize(const int nvectors, const int dim, double *x)
{
  for (int v = 0; v < nvectors; v++) {
    double length2 = sqr(x[v*dim]);
    for (int c = 1; c < dim; c++)
      length2 += sqr(x[v*dim+c]);
    double length = sqrt(length2);
    for (int c = 0; c < dim; c++)
      x[v*dim+c] /= length;
  }
}


void normalize(const long int nvectors, const long int dim, double *x)
{
  for (int v = 0; v < nvectors; v++) {
    double length2 = sqr(x[v*dim]);
    for (int c = 1; c < dim; c++)
      length2 += sqr(x[v*dim+c]);
    double length = sqrt(length2);
    for (int c = 0; c < dim; c++)
      x[v*dim+c] /= length;
  }
}


double spher_area4(const double *x, const long int corner[4], double spat[4])
{
  const double
    *a = x+ 3*corner[0],
    *b = x+ 3*corner[1],
    *c = x+ 3*corner[2],
    *d = x+ 3*corner[3],
    ab = dotproduct3(a,b),
    ac = dotproduct3(a,c),
    ad = dotproduct3(a,d),
    bc = dotproduct3(b,c),
    bd = dotproduct3(b,d),
    cd = dotproduct3(c,d),
    Ca = bd - ad*ab,
    Cb = ac - ab*bc,
    Cc = bd - bc*cd,
    Cd = ac - cd*ad;
  spat[0] = det3(d,a,b);  
  spat[1] = det3(a,b,c);
  spat[2] = det3(b,c,d);
  spat[3] = det3(c,d,a);
  double area =
    -atan2(Ca, spat[0]) -atan2(Cb, spat[1]) -atan2(Cc, spat[2]) -atan2(Cd, spat[3]);
  return fmod(area + 8.5 * M_PI, M_PI)- 0.5*M_PI;  // CVGIP => no time for deep analysis
} /* spher_area4 */



double spher_area4(const double *x, const int corner[4], double spat[4])
{
  const double
    *a = x+ 3*corner[0],
    *b = x+ 3*corner[1],
    *c = x+ 3*corner[2],
    *d = x+ 3*corner[3],
    ab = dotproduct3(a,b),
    ac = dotproduct3(a,c),
    ad = dotproduct3(a,d),
    bc = dotproduct3(b,c),
    bd = dotproduct3(b,d),
    cd = dotproduct3(c,d),
    Ca = bd - ad*ab,
    Cb = ac - ab*bc,
    Cc = bd - bc*cd,
    Cd = ac - cd*ad;
  spat[0] = det3(d,a,b);  
  spat[1] = det3(a,b,c);
  spat[2] = det3(b,c,d);
  spat[3] = det3(c,d,a);
  double area =
    -atan2(Ca, spat[0]) -atan2(Cb, spat[1]) -atan2(Cc, spat[2]) -atan2(Cd, spat[3]);
  return fmod(area + 8.5 * M_PI, M_PI)- 0.5*M_PI;  // CVGIP => no time for deep analysis
} /* spher_area4 */



void spher_step(double step, double *src, double *vec, long int nvect, double *dest)
{
  for (int i = 3*nvect; i--;)
    dest[i] = src[i] + step * vec[i];
  normalize(nvect, (long int) 3, dest);
}


void spher_step(double step, double *src, double *vec, int nvect, double *dest)
{
  for (int i = 3*nvect; i--;)
    dest[i] = src[i] + step * vec[i];
  normalize(nvect, 3, dest);
}


void copy_vector(double *dest, const double *src, const long int length)
{
  for (int i = 0; i < length; i++)
    dest[i] = src[i];
}


void copy_vector(double *dest, const double *src, const int length)
{
  for (int i = 0; i < length; i++)
    dest[i] = src[i];
}
