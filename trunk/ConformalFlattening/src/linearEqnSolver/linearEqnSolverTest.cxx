#include "linearEqnSolverTest.h"


int main() {

  vnl_sparse_matrix<double> A(5, 5);
  A(0,0) = 1;  A(0,1) = -0.3;  A(0,2) = -0.7; A(0,3) = 0; A(0, 4) = 0;
  A(1,0) = -0.3;  A(1,1) = 1.2;  A(1,2) = -0.6; A(1,3) = 0; A(1, 4) = 0;
  A(2,0) = -0.7;  A(2,1) = -0.6;  A(2,2) = 1.7; A(2,3) = -0.4; A(2, 4) = 0;
  A(3,0) = 0;  A(3,1) = 0;  A(3,2) = -0.4; A(3,3) = 1; A(3, 4) = -0.6;
  A(4,0) = 0;  A(4,1) = 0;  A(4,2) = 0; A(4,3) = -0.6; A(4, 4) = 0.6;

  vnl_vector<double> b(5);
  b(0) = 1;
  b(1) = 2;
  b(2) = 3;
  b(3) = 4;
  b(4) = 5;

  vnl_vector<double> c;

//   vnl_sparse_matrix<double> b(5, 1);
//   b(0, 0) = 1;
//   b(1, 0) = 2;
//   b(2, 0) = 3;
//   b(3, 0) = 4;
//   b(4, 0) = 5;

    
  //  linearEqnSolver les(A, b);
  
//   vcl_cerr<<vcl_endl<<"A="<<vcl_endl;
//   vcl_cerr<<A<<vcl_endl;

//   vcl_cerr<<"b="<<vcl_endl;
//   vcl_cerr<<b<<vcl_endl<<vcl_endl;


  //  vnl_vector<double> C;
  A.mult(b, c);
  vcl_cerr << c<<vcl_endl;
  //  vcl_cerr<<"Solving Ax=b by conjugate gradient method..."<<vcl_endl
  //      <<"The result is:"<<vcl_endl;

  //  vcl_cerr<<les.solve()<<vcl_endl<<vcl_endl;


  return 0;
}


