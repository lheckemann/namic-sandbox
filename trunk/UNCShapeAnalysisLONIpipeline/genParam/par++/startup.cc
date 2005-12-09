static const char SccsId[] = "@(#)startup.cc  3.5 03/24/93";
#include "utility.hh" 
//#include "CF_macros.h"
#include <math.h>
#include "vertex.hh"

//sparse matrix stuff
#include "compcol_double.h"              // Compressed column matrix header
#include "mvblasd.h"                     // MV_Vector level 1 BLAS
#include "diagpre_double.h"              // Diagonal preconditioner
#include "icpre_double.h"              // IC preconditioner
#include "ilupre_double.h"              // ILU preconditioner
#include "ir.h"                         // IML++ richardson template
#include "cheby.h"                        // IML++ chebyshev template 
#include "bicg.h"                       // IML++ biconjugate gradient template
#include "bicgstab.h"                        // IML++ biconjugate gradient stabilized  template 
#include "cg.h"                          // IML++ Conjugate gradient template
#include "cgs.h"                         // IML++ Conjugate gradient squared template
#include "qmr.h"                       // IML++ quasi minimum residual template
#include "gmres.h"                        // IML++ generalized minium residual template 

struct CompRows
{
  int n;
  int *ia;
  int *ja;
  double *a;
};

#define SQUARE(x) ((x)*(x))

extern  char  *progname;


void generate_matrix(struct CompRows *mat, const Net& net)
{
  const int  north_pole = 0,
    south_pole = net.nvert-1;
  int    n_nonzero = 0;
  for (int v = 1; v < south_pole; v++)
    n_nonzero+= net.vert[v].count / 2 + 1;        // + 1 diagonal element
  mat->n  = net.nvert-2;
  mat->ja = new int[n_nonzero];
  mat->ia = new int[mat->n+1  ];
  mat->a  = new double [n_nonzero];
  int n_used= 0;
  for (int v = 1; v < south_pole; v++) {
    Vertex *vertex = net.vert + v;
    mat->ia[v-1] = n_used;        // net_index = matrix_index+1
    mat->ja[n_used  ] = v-1;
    mat->a [n_used++] = vertex->count/2;    // put diagonal
    for (int nb = 0; nb < vertex->count; nb+= 2) {
      int neighbour= vertex->neighb[nb];
      if (neighbour != north_pole && neighbour != south_pole) {
  mat->ja[n_used  ] = neighbour-1;
  mat->a [n_used++] = -1.0;
      }
    }
  }
  mat->ia[mat->n] = n_used;
}



void
  modify_matrix(struct CompRows *mat, const Net& net)
  // modify the matrix for longitude computation by removing the
  // connections to the poles and increasing the diagonal element of an
  // arbitrary row (we select the first) by an arbitrary value (we select
  // 2.0) that makes the matrix nonsingular. This allows us to specify a
  // desired value for the longitude of vertex 1. Changing only the diagonal
  // element preserves the symmetry of the matrix.
{
  for (int p = 0; p < 2; p++) {
    Vertex *pole = net.vert + p*(net.nvert-1);    // north (p=0) or south (p=1)
    for (int nb = 0; nb < pole->count; nb+= 2)
      mat->a[mat->ia[pole->neighb[nb]-1]]--;
  }
  mat->a[0] += 2.0;          // make matrix regular: longi[1] = 0
}



void
  set_lati_rhs(const Net& net,   double* rhs)
{
  for (int i = 0; i < net.nvert; rhs[i++] = 0.0);
  Vertex *south= net.vert + net.nvert-1;
  for (int nb = 0; nb < south->count; nb+= 2)
    rhs[south->neighb[nb]]= M_PI;
}



static int*
  cycle2(int** pos, Vertex* vert)
     // Advance a pointer to any of vert's neighbour cursors
     // cyclically by 2 positions.
{
  *pos += 2;
  if (*pos >= vert->neighb + vert->count)
    *pos -= vert->count;
  return *pos;
}


void
  set_longi_rhs(const Net& net, double * rhs, double * lati)
{
  for (int i = 0; i < net.nvert; i++) rhs[i] = 0;
  int prev = 0;            // northpole
  int here = 1;                        // any neighbor of northpole
  double maximum = 0.0;
  while (here != net.nvert-1) {
    Vertex *hereV = net.vert+here;
    int *maxpos = 0, *prevpos = 0;
    for (int *nb = hereV->neighb; nb < hereV->neighb+hereV->count; nb += 2)
      if (lati[*nb] > maximum) {
  maximum = lati[*nb];
  maxpos = nb; }
      else if (*nb == prev)
  prevpos = nb;
    for (int *nb= prevpos; cycle2(&nb, hereV) != maxpos;) {
      rhs[*nb ] += 2.0*M_PI;
      rhs[here] -= 2.0*M_PI;        // totally: 2 pi * # east_neighbours
    }
    prev = here;
    here = *maxpos;
  }
}



void
  start_values(const Net& net, double *cartesian)
{
  struct CompRows mat;


  double *rhs  = new double [net.nvert];
  double *lati = new double [net.nvert];
  double *longi = new double [net.nvert];
  int iter;
  double tol =  1e-14;
  int n_nonzero;

  generate_matrix(&mat, net);
  set_lati_rhs(net, rhs);
  n_nonzero = mat.ia[mat.n];
  long t1 = time(NULL);

  CompRow_Mat_double A(mat.n, mat.n, n_nonzero, mat.a, mat.ia, mat.ja); 
  // call of compressed row matrix = nxn with n_nonzero items that are non-zero, mat.a = value vector,
  // mat.ia = row pointer, mat.ja = column index
  // Build up the structure from a grid in 0-relative compressed row format

  ICPreconditioner_double D(A);
  //CompRow_ILUPreconditioner_double D(A);
  // DiagPreconditioner_double D(A);
  
  iter = 5000; // max iter, actual iterations returned in this variable
  
  VECTOR_double b(mat.n,0.0);
  VECTOR_double x(mat.n,0.0);
  for (int i = 0; i < mat.n; i++) b[i] = rhs[i+1]; // copy rhs

  int result = CG(A, x, b, D ,iter, tol); // A * x = b, max it, tol  6
  //int result = CGS(A, x, b, D ,iter, tol); // A * x = b, max it, tol BAD
  //int result = IR(A, x, b, D ,iter, tol); // A * x = b, max it, tol
  //int result = BiCGSTAB(A, x, b, D ,iter, tol); // A * x = b, max it, tol 9
  //int result = BiCG(A, x, b, D ,iter, tol); // A * x = b, max it, tol 12

  cout << "flag = " << result <<  " iterations performed: " << iter << endl;

  for (int i = 0; i < mat.n; i++) lati[i+1] = x[i]; //copy result
  lati[0] = 0.0;
  lati[net.nvert-1] = M_PI;

  modify_matrix(&mat, net);
  set_longi_rhs(net, rhs, lati);
  n_nonzero = mat.ia[mat.n];
  CompRow_Mat_double A2(mat.n, mat.n, n_nonzero, mat.a, mat.ia, mat.ja);

  ICPreconditioner_double D2(A2);
  //CompRow_ILUPreconditioner_double D2(A2);
  //DiagPreconditioner_double D2(A2);
 
  tol = 1e-14;
  iter = 5000; // max iter, actual iterations returned in this variable

  for (int i = 0; i < mat.n; i++) b[i] = rhs[i+1]; // copy rhs

  int result2 = CG(A2, x, b, D2 ,iter, tol); // A2 * x = b, max it, tol
  //int result2 = CGS(A2, x, b, D2 ,iter, tol); // A2 * x = b, max it, tol
  //int result2 = IR(A2, x, b, D2 ,iter, tol); // A2 * x = b, max it, tol
  //int result2 = BiCGSTAB(A2, x, b, D2 ,iter, tol); // A2 * x = b, max it, tol
  //int result2 = BiCG(A2, x, b, D2 ,iter, tol); // A2 * x = b, max it, tol

  cout << "flag = " << result2 <<  " iterations performed: " << iter << endl;
  long t2 = time(NULL);
  cout << "time = " << t2-t1<< endl;

  for (int i = 0; i < mat.n; i++) longi[i+1] = x[i]; //copy result
  longi[0] = longi[net.nvert-1] = 0.0;

  for (int i= 0; i < net.nvert; i++) {
    cartesian[i*3+0] = sin(lati[i])*cos(longi[i]);
    cartesian[i*3+1] = sin(lati[i])*sin(longi[i]);
    cartesian[i*3+2] = cos(lati[i]);
  }
}
