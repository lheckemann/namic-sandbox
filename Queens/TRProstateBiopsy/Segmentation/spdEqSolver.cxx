#include "spdEqSolver.h"

#include <float.h>  // for DBL_MIN

#include <iostream>


bool isPosDef(mtx* A)
{
  vnl_symmetric_eigensystem<elementType> eig(*A);

  // the eigen system will return the smallest eigen value in the
  // 0-th place:
  elementType minLambda = eig.get_eigenvalue(0);

  if (minLambda <= 0)
    {
      return false;
    }
  
  return true;
}

bool isPosDef(spMtx* A)
{
  vnl_sparse_symmetric_eigensystem eig;
  eig.CalculateNPairs((*A), 1);

  // the eigen system will return the smallest eigen value in the
  // 0-th place:
  elementType minLambda = eig.get_eigenvalue(0);

  if (minLambda <= 0)
    {
      return false;
    }
  
  return true;
}


int solveSpdEq(mtx* A, vec* b, vec* x, int numIter, elementType tol)
{
//   if (!isPosDef(A))
//     {
//       std::cerr<<"NOT positive definite! exiting...\n";
//       exit(-1);
//     }

  vec r = (*b) - (*A)*(*x);
  vec p = r;
  
  if (numIter == -1)
    {
      numIter = b->size();
      numIter += numIter/10; // let the iteration times a little more than the dimesion
    }

  for (int i = 0; i <= numIter; ++i)
    {
      vec Ap = (*A)*p;

      elementType pAp = inner_product(p, Ap);

      elementType alpha = inner_product(p, r)/(pAp + DBL_MIN);

      (*x) += alpha*p;

      r -= alpha*Ap;
      if (inner_product(r, r) < tol)
        {
          return i;
        }

      vec Ar = (*A)*r;

      elementType beta = -inner_product(p, Ar)/(pAp + DBL_MIN);

      p = r + beta*p;
    }

  return numIter;
}


int solveSpdEq(spMtx* A, vec* b, vec* x, int numIter, elementType tol)
{
//   std::cout<<"in sparse matrix solver!\n"<<std::flush;
//   std::cout<<"numIter = "<<numIter<<"\n"<<std::flush;
//   std::cout<<"tol = "<<tol<<"\n"<<std::flush;

//   if (!isPosDef(A))
//     {
//       std::cerr<<"NOT positive definite! exiting...\n";
//       exit(-1);
//     }

//   std::cout<<"max element of *b = "<<b->max_value()<<std::endl<<std::flush;
//   std::cout<<"b is zero?"<<b->is_zero()<<std::endl<<std::flush;


  vec Ax;
  A->pre_mult(*x, Ax);

  vec r = (*b) - Ax;
  vec p = r;
  
  if (numIter == -1)
    {
      numIter = b->size();
      numIter += numIter/10; // let the iteration times a little more than the dimesion
    }

  for (int i = 0; i <= numIter; ++i)
    {
      vec Ap;
      A->pre_mult(p, Ap);

      elementType pAp = inner_product(p, Ap);
      // std::cout<<"pAp = "<<pAp<<std::endl<<std::flush;


      elementType alpha = inner_product(p, r)/(pAp + DBL_MIN);
      // std::cout<<"alpha = "<<alpha<<std::endl<<std::flush;

      (*x) += alpha*p;

      r -= alpha*Ap;

      if (inner_product(r, r) < tol)
        {
          // std::cout<<"<r, r> = "<<inner_product(r, r)<<std::endl<<std::flush;
          return i;
        }

      vec Ar;
      A->pre_mult(r, Ar);

      elementType beta = -inner_product(p, Ar)/(pAp + DBL_MIN);

      p = r + beta*p;
    }

  return numIter;
}

