static const char SccsId[] = "@(#)sparsemat.cc  3.9 23 Jul 1994";

#include "utility.hh"
#include "sparsemat.hh"

#include <iostream>

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
  
sparseMatrix::sparseMatrix(int maxRow, int maxCol, int maxNonzero)
{
  max_col = maxCol;
  max_row = maxRow;
  max_nonzero = maxNonzero;
  n_row   = n_col = 0;
  ia      = new int[max_row+1];
  ja      = new int[max_nonzero];
  iaT     = 0;
  jaT     = 0;
  rowT    = 0;
  a       = new double[max_nonzero];
}



sparseMatrix::~sparseMatrix()
{
  delete ja;
  delete ia;
  delete iaT;
  delete jaT;
  delete rowT;
  delete a;
}



void sparseMatrix::from_net(const Net& net, int n_active, int *active)
{
  n_row = net.nface-1 + n_active;
  n_col = 3*net.nvert;
  //assert(n_row <= max_row, "too many rows in from_net.");
  //assert(n_col <= max_col, "too many columns in from_net.");
  //assert(3*(4*(net.nface-1) + 3*n_active) <= max_nonzero,
  //   "too many nozeros in from_net.");

  int in_pos = 0;
  int i;
  for (i = 0; i < net.nface-1; i++) {
    ia[i] = in_pos;
    for (int corner = 0; corner < 4; corner++) {
      int vertNr = net.face[4*i+ corner];
      for (int coord= 0; coord < 3; coord++)
  ja[in_pos++] = 3*vertNr+coord;
    }
  }
  for (i = 0; i < n_active; i++) {
    ia[i+net.nface-1] = in_pos;
    int faceNr = active[i]/4;
    for (int corner = 0; corner < 4; corner++)
      if ((active[i] + 4 - corner)%4 !=2) {    // opposite corner has no influence
  int vertNr = net.face[4*faceNr+ corner];
  for (int coord= 0; coord < 3; coord++)
    ja[in_pos++] = 3*vertNr+coord;
      }
  }
  ia[n_row] = in_pos;
  //assert(in_pos == 3*(4*(net.nface-1) + 3*n_active), "wrong in_pos count in from_net");
}



void sparseMatrix::invTables()
{
  if (!iaT) {
    iaT  = new int[max_col+1];
    jaT  = new int[max_nonzero];
    rowT = new int[max_nonzero];
  }
  int col,j;
  for ( col= 0; col <= n_col; iaT[col++] = 0); // initialize bins to 0
  for ( j = 0; j < ia[n_row]; iaT[ja[j++]]++); // column histogram
  for (col = 0; col < n_col; col++)      // sum the histogram
    iaT[col+1] += iaT[col];        // iaT points to last entry
  for (int i = n_row; i--; )        // backwards: last row first
    for (j = int(ia[i]); j < ia[i+1]; j++) {
      jaT [--iaT[ja[j]]] = j;        // also backwards => cancels => forward
      rowT[  iaT[ja[j]]] = i;
    }
}              // iaT points to first entry now



void sparseMatrix::mult (double *vec, double *result)
{
  check_finite(vec);
  for (int row = 0; row < n_row; row++) {
    double res = 0.0;
    for (int j = ia[row]; j < ia[row+1]; j++)
      res += a[j]*vec[ja[j]];
    result[row] = res;
  }
  check_finite(result);
}



void sparseMatrix::multT(double *vec, double *result)
{
  check_finite(vec);
  for (int col = 0; col < n_col; result[col++] = 0.0);
  for (int row = 0; row < n_row; row++)
    for (int j = ia[row]; j < ia[row+1]; j++)
      result[ja[j]] += a[j] * vec[row];
  check_finite(result);
}



void sparseMatrix::solve(int structure_change, double *rhs, double *x)
{
  check_finite(rhs);
  //assert(n_row == n_col, "Matrix must be square in sparseMatrix::solve");

  int n_nonzero = ia[n_row];

  CompRow_Mat_double A(n_row, n_row, n_nonzero, a, ia, ja); 
  // call of compressed row matrix = nxn with n_nonzero items that are non-zero, a = value vector,
  // ia = row pointer, ja = column index
  // Build up the structure from a grid in 0-relative compressed row format

  long t1 = time(NULL);
  //DiagPreconditioner_double D(A);
  ICPreconditioner_double D(A);
  
  int iter = 5000;
  double tol =  1e-14;
  
  VECTOR_double b(n_row,0.0);
  VECTOR_double sol(n_row,0.0);
  for (int i = 0; i < n_row; i++) b[i] = rhs[i]; // copy rhs

  int result = CG(A, sol, b, D ,iter, tol); // A * x = b, max it, tol
  //int result = BiCGSTAB(A, sol, b, D ,iter, tol); // A * x = b, max it, tol 9
  long t2 = time(NULL);
  cout << " t" << t2-t1 << " " << result <<  " i" << iter;
  for (int i = 0; i < n_row; i++) x[i] = sol[i]; //copy result

  check_finite(x);
}



void sparseMatrix::set_aTa(const sparseMatrix& aT)
{
  n_row = n_col = aT.n_row;
  //assert(n_row <= max_row, "too many rows in set_aTa.");
  //assert(n_col <= max_col, "too many columns in set_aTa.");
  //assert(aT.iaT != 0, "inverse tables must be set up before set_aTa");
  check_finite(aT.a);

  int inpos = 0;
  int j0,j2;
  for (int row = 0; row < n_row; row++) {
    ia[row] = inpos;
    for ( j0 = aT.ia[row]; j0 < aT.ia[row+1]; j0++) {
      int mix = aT.ja[j0];
      for (int j1 = aT.iaT[mix]; j1 < aT.iaT[mix+1]; j1++) {
  int col = aT.rowT[j1];
  ja[inpos] = col;
  for ( j2 = ia[row]; ja[j2] != col; j2++); // search for col
  if (j2 == inpos) a[inpos++] = 0.0;
  a[j2] += aT.a[j0] * aT.a[aT.jaT[j1]];
      } // j1
    } // j0
    //assert (inpos < max_nonzero, "too many nonzeros in set_aTa.");
  } // row
  ia[n_row] = inpos;
  check_finite(a);
}



void sparseMatrix::test_matrix()
{
  static int
    t_ia[5 ] = {0,             4,             8,         11,       14},
    t_ja[14] = {0, 3, 5, 6,    0, 1, 4, 6,    1, 2, 4,    0, 3, 5};
  static double
    t_a [14] = {2, 1, 3, 4,    1, 1, 2, 1,    4, 4, 3,    3, 2, 1};

  //assert(max_row >= 4, "Not enough rows in sparseMatrix::test_matrix");
  //assert(max_col >= 7, "Not enough columns in sparseMatrix::test_matrix");
  //assert(max_nonzero >= 14, "Not enough non-zeros in sparseMatrix::test_matrix");
  n_row = 4;
  n_col = 7;

  for (int i = 0; i <= n_row   ; i++)
    ia[i] = t_ia[i];
  for (int j = 0; j < ia[n_row]; j++) {
    ja[j] = t_ja[j];
    a [j] = t_a [j];
  }
}



void sparseMatrix::print(char* name, const int append)
{
  cout << "Printing of sparse matrix is not yet implemented..." << endl;
}
