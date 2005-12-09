static const char SccsId_sparsemat[] = "@(#)sparsemat.hh  3.2 17 Mar 1993";

#include "vertex.hh"  



class sparseMatrix {
  int    max_col, max_row, max_nonzero;

public:
  int      n_row, n_col, *iaT, *jaT, *rowT;
  int    *ia, *ja;
  double    *a;
  
  
  sparseMatrix(int, int, int);
  ~sparseMatrix();
  void from_net(const Net&, int n_active,  int *active_scatter);
  void invTables();          // set up inverted tables
  void mult (double *vec, double *result);    // multiply this.vec
  void multT(double *vec, double *result);    // multiply this^T.vec
  void solve(int change, double *rhs, double *x); // solve this.x == rhs
  void set_aTa(const sparseMatrix& aT);      // set this = aT . a
  void print(char* name, const int append);    // writes matrix to cout
  void test_matrix();
};
