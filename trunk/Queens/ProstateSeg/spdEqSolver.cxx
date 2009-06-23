#include "spdEqSolver.h"

#include <float.h>  // for DBL_MIN

#include <iostream>


SpdEqSolver::SpdEqSolver()
{
  _progressCallback=NULL;
}


SpdEqSolver::~SpdEqSolver()
{
}

bool SpdEqSolver::isPosDef(mtx* A)
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

bool SpdEqSolver::isPosDef(spMtx* A)
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


int SpdEqSolver::solveSpdEq(mtx* A, vec* b, vec* x, int numIter, elementType tol)
{
  //   if (!isPosDef(A))
  //     {
  //       std::cerr<<"NOT positive definite! exiting...\n";
  //       exit(-1);
  //     }

  vec r = (*b) - (*A)*(*x);
  vec p = r;

  reportProgress(0);

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
          reportProgress(100);
          return i;
        }

      vec Ar = (*A)*r;

      elementType beta = -inner_product(p, Ar)/(pAp + DBL_MIN);

      p = r + beta*p;
      
      reportProgress(i*100/numIter);
    }

  reportProgress(100);
  return numIter;
}




/* Jacobi Preconditioned CG */
/*============================================================*/
int SpdEqSolver::solveSpdEq(spMtx* A, vec* b, vec* x, int numIter, elementType tol)
{
  reportProgress(0);

  vec Ax;
  A->pre_mult(*x, Ax);
  //  std::cout<<"*x is zero? "<<x->is_zero()<<std::endl<<std::flush;
  //  std::cout<<"Ax is zero? "<<Ax.is_zero()<<std::endl<<std::flush;


  unsigned long N = A->cols();
  if (A->rows() != N)
    {
      std::cerr<<"A is not square, abort. \n"<<std::endl;
      exit(-1);
    }

  vec r = (*b) - Ax;

  vec z( r.size() );

  // Jacobi preconditioner
  vec Cinv( r.size() );
  for (unsigned long i = 0; i < N; ++i)
    {
      Cinv[i] = 1.0/( (*A)(i, i) + DBL_MIN);

      z[i] = r[i]*Cinv[i];
    }

  vec d = z;

  
  if (numIter == -1)
    {
      numIter = b->size();
      numIter += numIter/10; // let the iteration times a little more than the dimesion
    }

  for (int i = 0; i <= numIter; ++i)
    {
      vec Ad;
      A->pre_mult(d, Ad);

      elementType dAd = inner_product(d, Ad);

      elementType zTr = inner_product(z, r);

      elementType alpha = zTr/(dAd + DBL_MIN);

      (*x) += alpha*d;

      r -= alpha*Ad;

      double rTr = inner_product(r, r);
      if ( rTr < tol)
        {
          // std::cout<<"<r, r> = "<<rTr<<std::endl<<std::flush;
          reportProgress(100);
          return i;
        }

      for (unsigned long id = 0; id < N; ++id)
        {
          z[i] = r[i]*Cinv[i];
        }

      elementType beta \
        = inner_product(z, r)/(zTr + DBL_MIN);


      d = z + beta*d;

      reportProgress(i*100/numIter);
    }

  reportProgress(100);
  return numIter;
}


// int SpdEqSolver::solveSpdEq(spMtx* A, vec* b, vec* x, int numIter, elementType tol)
// {
// //   std::cout<<"in sparse matrix solver!\n"<<std::flush;
// //   std::cout<<"numIter = "<<numIter<<"\n"<<std::flush;
// //   std::cout<<"tol = "<<tol<<"\n"<<std::flush;

// //   if (!isPosDef(A))
// //     {
// //       std::cerr<<"NOT positive definite! exiting...\n";
// //       exit(-1);
// //     }

// //   std::cout<<"max element of *b = "<<b->max_value()<<std::endl<<std::flush;
// //   std::cout<<"b is zero?"<<b->is_zero()<<std::endl<<std::flush;

//   reportProgress(0);

//   vec Ax;
//   A->pre_mult(*x, Ax);
//   //  std::cout<<"*x is zero? "<<x->is_zero()<<std::endl<<std::flush;
//   //  std::cout<<"Ax is zero? "<<Ax.is_zero()<<std::endl<<std::flush;


//   vec r = (*b) - Ax;
//   vec p = r;
  
//   if (numIter == -1)
//     {
//       numIter = b->size();
//       numIter += numIter/10; // let the iteration times a little more than the dimesion
//     }

//   for (int i = 0; i <= numIter; ++i)
//     {
//       vec Ap;
//       A->pre_mult(p, Ap);

//       elementType pAp = inner_product(p, Ap);
//       // std::cout<<"pAp = "<<pAp<<std::endl<<std::flush;


//       elementType alpha = inner_product(p, r)/(pAp + DBL_MIN);
//       //std::cout<<"alpha = "<<alpha<<std::endl<<std::flush;

//       (*x) += alpha*p;

//       r -= alpha*Ap;

//       if (inner_product(r, r) < tol)
//         {
//           //          std::cout<<"<r, r> = "<<inner_product(r, r)<<std::endl<<std::flush;
//           reportProgress(100);
//           return i;
//         }

//       vec Ar;
//       A->pre_mult(r, Ar);

//       elementType beta = -inner_product(p, Ar)/(pAp + DBL_MIN);

//       p = r + beta*p;
//       reportProgress(i*100/numIter);
//     }

//   reportProgress(100);
//   return numIter;
// }




void SpdEqSolver::setProgressCallback(PROGRESS_CALLBACK_TYPE progressCallback)
{
  _progressCallback=progressCallback;
}

void SpdEqSolver::reportProgress(int percentComplete)
{
  if (_progressCallback!=NULL)
    {
      (*_progressCallback)(percentComplete);
    }
}

