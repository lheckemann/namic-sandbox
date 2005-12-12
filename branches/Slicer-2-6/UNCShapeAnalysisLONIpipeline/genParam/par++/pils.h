/***********************************************************************/
/*                           The IIS Solver                            */
/*     Integrated Systems Laboratory, ETH Zurich, Switzerland, 1990    */
/*---------------------------------------------------------------------*/
/*   Filename:  pils.h                                                 */
/*   Purpose:   a collection of external interfaces to the PILS solver */
/*   Language:  ANSI C (header file)                                   */
/*   Author:    Claude Pommerell                                       */
/*   Created:   Tue Nov 13 18:57:24 MET 1990 (cp)                      */
/***********************************************************************/
/* $Id: pils.h,v 1.60 93/06/30 16:59:39 pommy Exp $ */
#include <Machine.h>
#include <CF_macros.h>
#ifndef PILSH
#define PILSH


#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------*/
/* PILS interface #1:                                                     */
/* =================                                                      */

/* What we really want: The user defines the structure of the grid        */
/* through a YSMP representation, knows how long the arrays for the       */
/* matrix, the right-hand side, and the solution array have to be, and    */
/* is happy with the ordering we offer.                                   */
/* Note that, since the some informations required are not visible        */
/* through the present header file, no external client program can rely   */
/* on the this interface only, at the time being. Some of the alternative */
/* interfaces offer construction helps that make it possible to use       */
/* some of the access functions in the this interface.                    */
/* The PILS test driver currently uses (only) this interface.             */

/***************************** I M P O R T A N T ***************************/
/*                                                                         */
/* Conventions for argument passing:                                       */
/* ================================                                        */
/* Formal parameters                                                       */
/* -----------------                                                       */
/* Any C function "c_function" that accesses a sparse matrix must declare  */
/* its formal parameters as                                                */
/* RETURN_TYPE                                                             */
/*   c_function (OTHER_PARAMETERS_BEFORE,                                  */
/*               const SparseMatrixStructure *structure,                   */
/*               const NORMAL_PRECISION matrixA [],                        */
/*               OTHER_PARAMETERS_AFTER);                                  */
/* The "const" specifier for "structure" is mandatory.                     */
/* The "const" specifier for "matrixA" is mandatory unless the function    */
/* modifies the some nonzero entries of the matrix.                        */
/* If the function accesses a second matrix with the same structure,       */
/* another formal parameter "const NORMAL_PRECISION *matrixB" is given,    */
/* without putting another "SparseMatrixStructure" argument.               */
/*                                                                         */
/* A C++ function "plus_function" that accesses a sparse matrix            */
/* should declare its formal parameters as                                 */
/* RETURN_TYPE                                                             */
/*   plus_function (OTHER_PARAMETERS_BEFORE,                               */
/*                  const SparseMatrixStructure &structure,                */
/*                  const NORMAL_PRECISION matrixA [],                     */
/*                  OTHER_PARAMETERS_AFTER);                               */
/*                                                                         */
/* Any Fortran subroutine "F_SUB" that accesses a sparse matrix must       */
/* declare its formal parameters as                                        */
/*       SUBROUTINE F_SUB (before, STRUCT, MATRIXA, after)                 */
/*       other_parameter before                                            */
/*       int (*) STRUCT                                                */
/*       normal_precision (*) MATRIXA                                      */
/*       other_parameter after                                             */
/* If the subroutine accesses a second matrix with the same structure,     */
/* another formal parameter of the type "DOUBLE PRECISION (*) MATRIXB"     */
/* is given, without putting another "STRUCT" argument.                    */
/*                                                                         */
/* Actual arguments                                                        */
/* ----------------                                                        */
/* Any C program passing a sparse matrix to a function "callee" must       */
/* be in the scope of the declarations                                     */
/*         SparseMatrixStructure *structure;                               */
/*         NORMAL_PRECISION *matrixA;                                      */
/* It will pass the matrix as actual argument to the function by calling   */
/*         callee (BEFORE, structure, matrixA, AFTER);                     */
/*                                                                         */
/* Any Fortran program passing a sparse matrix to a subroutine "CALLEE"    */
/* must be in the scope of the declarations                                */
/*         int (*) STRUCT                                              */
/*         normal_precision (*) MATRIXA                                    */
/* It will pass the matrix as actual argument to the subroutine by calling */
/*         CALL CALLEE (before, STRUCT, MATRIXA, after)                    */
/*                                                                         */
/***************************************************************************/

struct SparseMatrixStructure;

extern void
  DefStructureFromYSMP (const int n,
      int *ia, int *ja, /* 0-relative YSMP */
      struct SparseMatrixStructure **structure_P,
      int **new2old_P);

/* Build up the structure from a grid in 0-relative compressed row     */
/* storage (YSMP format).                                              */
/* On exit, (*structure_P) points to a structure to be used in further */
/* access calls, and (*new2old_P) points to a permutation relating the */
/* PILS ordering ("new") to the original ordering ("old").             */

extern void
  DestroyStructure (struct SparseMatrixStructure *structure);
/* Release the memory allocated for "structure". */

extern void
  SetDiagonal (struct SparseMatrixStructure *structure,
         NORMAL_PRECISION matrixA [],
         const NORMAL_PRECISION values []);
/* Overwrites the diagonal of the matrix with the contents of "values". */

extern void
  GetDiagonal (struct SparseMatrixStructure *structure,
         const NORMAL_PRECISION matrixA [],
         NORMAL_PRECISION values []);
/* Overwrites "values" with the diagonal of the matrix. */

extern void
  AccessOffDiagonals (struct SparseMatrixStructure *structure,
          NORMAL_PRECISION matrixA [],
          void (* Iterator) (const int *rowFrom,
           const int *length,
           const int colIndex [],
           NORMAL_PRECISION lowerData [],
           NORMAL_PRECISION upperData [],
           int *otherInts,
           NORMAL_PRECISION *otherNormals),
          int *otherInts,
          NORMAL_PRECISION *otherNormals);
/* Calls "Iterator" repeatedly to access the offdiagonals.        */
/* At each call, "Iterator" gets new values for its first five        */
/* parameters satisfying the follwing criteria:            */
/* - "(*rowFrom) >= 0" and                */
/*   "(*rowFrom) + (*length) < structure->noOfVertices"          */
/* - For each "k" such that "0 <= k < (*length)", let "I" and "J" be  */
/*   defined as "I = *(*rowFrom) + k, J = colIndex[k]". Then        */
/*    - "I < J <= structure->noOfVertices" holds          */
/*    - if "J != structure->noOfVertices",            */
/*  then A[J][I] and A[I][J] are nonzero entries of the matrix,   */
/*  and "lowerData[k]" and "upperData[k]" hold their values.      */
/*    - if "J == structure->noOfVertices", then "lowerData[k]" and    */
/*  "upperData[k]" may take any value.            */
/* - "colIndex[0:(*length)-1]" is a permutation vector, that is, no   */
/*   entry (except the dummy entry "structure->noOfVertices") appears */
/*   more than once.                  */
/* - Every nonzero off-diagonal entry of the matrix appears exactly   */
/*   once in a call to "Iterator".              */
/* - "otherInts" and "otherNormals" get passed through to "Iterator"  */
/*   without any change.                */
/* The caller may write "Iterator" in such a way that it overwrites   */
/* the two arrays "lowerData" and "upperData". A loop over "k" with   */
/* "0 <= k < (*length)" can be fully vectorized and parallelized. The */
/* additional parameters "otherInts" and "otherNormals" may convey    */
/* other data required by the "Iterator" to perform its task.        */


struct IterationStatus
{
  bool success;
  int numberOfIterations;
  NORMAL_PRECISION resultAccuracy;
};


extern void
  SolveSystem (struct SparseMatrixStructure *structure,
         NORMAL_PRECISION matrixA [],
         NORMAL_PRECISION vectorB [],
         NORMAL_PRECISION vectorX [],
         struct IterationStatus *status);
/* Solve a linear system Ax=b, where the sparsity structure of the     */
/* matrix ("structure"), the values of the matrix ("matrixA"), and the */
/* values of the right-hand side are given ("vectorB").                */
/* "vectorX" should be initialized to an initial guess known by the    */
/* client application. If no such guess is available, "vectorX" should */
/* be initialized to zero.                                             */
/* On return, "vectorX" holds the approximate solution, and "status"   */
/* holds some informations about the iteration process.                */


extern void
  StringOfOptions (char *string);
/* enter options to the solver in form of a string. */

extern void
  FileOfOptions (char *filename);
/* enter options to the solver in form of a file. */


extern void
  SetTolerance (const NORMAL_PRECISION tolerance);
/* Set the convergence tolerance.                          */
/* Equivalent to                                           */
/*   StringOfOptions(sprintf(str,                          */
/*                           "Convergence-Tolerance = %g", */
/*                           tolerance))                   */


extern void
  SetMaxIter (const int maxiter);
/* Set the maximum number of "iterations".                        */
/* Equivalent to                                                  */
/*   StringOfOptions(sprintf(str,                                 */
/*                           "Maximum-Number-of-Iterations = %d", */
/*                           maxiter))                            */


extern void
  (* PilsPrint) (const char *str);
/* This function can be reassigned by a client application. */
/* All text output from PILS goes through this function.    */
/* By default, "PilsPrint" points to a function that        */
/* prints to standard output.                               */


/*--------------------------------------------------------------------*/

/* PILS interface #2:                                                 */
/* =================                                                  */

/* Permutation index vectors define how internal indexing of vertices */
/* and edges is mapped to external indexing of vertices and edges.    */
/* Once the "ISInt2ExtInterface" is constructed, the client calls     */
/* "I2E_SolveSystem" which first copies arrays into the format        */
/* required internally in PILS.                                       */


struct ISInt2ExtInterface;

#define I2E_InactiveVertex (-1)

typedef
  enum {MSF_Diagonal, MSF_Symmetric, MSF_StructurallySymmetric}
MatrixSymmetryForm;

struct VectorBlockDefinition
{
  int noOfActiveExtVertices;
  int *allext2activeVertices; /* [noOfAllExtVertices] */
  int *active2allextVertices; /* [noOfActiveExtVertices] */
  int vectorOffset;
};
/* Defines an extract of a vector by listing all active entries        */
/* ("vertices") in "active2allextVertices".                            */
/* "allext2activeVertices" contains the inversion of the list          */
/* "active2allextVertices", i.e.                                       */
/* "allext2activeVertices[active2allextVertices[i]] == i".             */
/* "allext2activeVertices" holds "I2E_InactiveVertex" for all vertices */
/* not occuring in "active2allextVertices".                            */
/* "vectorOffset" defines where this block of the vector starts in the */
/* data arrays passed along with I2E_SolveSystem.                      */

struct MatrixBlockDefinition
{
  int noOfAllExtEdges;
  int *connList_fromExtVertex; /* [noOfAllExtEdges] */
  int *connList_toExtVertex;   /* [noOfAllExtEdges] */
  MatrixSymmetryForm symmetry;
  int diagOffset;
    /* start index in the matrix data array of the diagonal of this block */
  int fromtoOffset;
    /* start index in the matrix data array of the edges "a[from][to]". */
    /* Undefined if "symmetry == MSF_Diagonal".                         */
  int tofromOffset;
    /* start index in the matrix data array of the edges "a[to][from]".      */
    /* Undefined if "symmetry == MSF_Diagonal"                               */
    /* "symmetry == MSF_Symmetric" must imply "fromtoOffset == tofromOffset" */
};
/* Defines the structure of one block of a matrix.                      */
/* The connection list in "connList_fromVertex" and "connList_toVertex" */
/* defines the positions of the offdiagonal nonzero entries.            */
/* For "symmetry == MSF_StructurallySymmetric", if all the edges        */
/* in the connection list are defined upwards, such that "from < to",   */
/* then "fromtoOffset" starts the lower triangle of this block, and     */
/* "tofromOffset" starts the upper triangle of this block.              */

struct BlockedMatrixDefinition
{
  int noOfBlocks;
  int noOfAllExtVertices;
  struct VectorBlockDefinition *vecBlock; /* [noOfBlocks] */
  struct MatrixBlockDefinition **matBlock; /* [noOfBlocks][noOfBlocks] */
};
/* Defines the structure of a blocked matrix. */

extern struct ISInt2ExtInterface *
  ConstructFromConnectionLists (struct BlockedMatrixDefinition *bmd);
/* Construct the structure of a blocked matrix */

extern struct ISInt2ExtInterface *
  ConstructFromYSMP (const int n, int ia [], int ja []);
/* Construct the structure of a 0-relative compressed row storage format */
/* (ysmp format).                                                        */
/* The matrix must be structurally symmetric, and every compressed row   */
/* must contain a diagonal entry.                                        */

extern void
  DestroyISInt2ExtInterface (struct ISInt2ExtInterface *i2e);
/* Release the memory allocated for "i2e". */

extern void
  I2E_SolveSystem (struct ISInt2ExtInterface *i2e,
       NORMAL_PRECISION *extMatrixA,
       NORMAL_PRECISION *extVectorB,
       NORMAL_PRECISION *extVectorX);
/* Solve a linear system Ax=b. Matrix and vectors are given in */
/* external indexing, corresponding to "i2e".                  */
/* See also "SolveSystem", which is called internally.         */

extern void
  I2E_MVMult (struct ISInt2ExtInterface *i2e,
        NORMAL_PRECISION *extMatrixA,
        NORMAL_PRECISION *extVectorV,
        NORMAL_PRECISION *extVectorW);
/* Compute a matrix-vector product: "w = A * v" */

extern void
  I2E_RowSum (struct ISInt2ExtInterface *i2e,
     NORMAL_PRECISION *extOffdiagA,
     NORMAL_PRECISION *extRowSum);
/* Compute the offdiagonal row sums of a matrix:           */
/* "rowSum = (A - diag(A)) * (1 1 ... 1)^T"                */
/* Note that the diagonal of the matrix A is ignored here. */

extern void
  I2E_ColumnSum (struct ISInt2ExtInterface *i2e,
     NORMAL_PRECISION *extOffdiagA,
     NORMAL_PRECISION *extColumnSum);
/* Compute the offdiagonal column sums of a matrix:        */
/* "columnSum = (A - diag(A))^T * (1 1 ... 1)^T"           */
/* Note that the diagonal of the matrix A is ignored here. */
  
extern NORMAL_PRECISION
  I2E_ResultAccuracy (struct ISInt2ExtInterface *i2e);
/* Return the obtained accuracy of the solution (according to the */
/* convergence criterium use in the iteration process).           */


extern int
  I2E_NumberOfIterations (struct ISInt2ExtInterface *i2e);
/* Return a figure more or less monotonically related to the amount of */
/* work in the iteration process.                                      */


extern void
  (* ExternalPrecond) (const int length,
           const int map [],
           NORMAL_PRECISION v []);
/* This function pointer should be reassigned by a client application   */
/* willing to provide external preconditioning.                         */
/* "Q" being the preconditioner and "v" the unpreconditioned input      */
/* vector, this function should return in "v" the preconditioned        */
/* vector "Q^-1 v".                                                     */
/* "map" is an index vector of length "length" that maps PILS numbering */
/* to external numbering. The client stores "v[i] as "ext_v[map[i]".    */
/* Once this function is defined, external preconditioning can be       */
/* selected through                                                     */
/*   StringOfOptions("precond = external")                              */

extern void
  (* ExternalTPrecond) (const int length,
      const int map [],
      NORMAL_PRECISION v []);
/* This function needs to be reassigned by a client application only if */
/* external preconditioning is used with the BiCG or CGNR methods.      */
/* "Q" being the preconditioner and "v" the unpreconditioned input      */
/* vector, this function should return in "v" the transposedly          */
/* preconditioned vector "Q^-T v".                                      */
/* "map" is an index vector of length "length" that maps PILS numbering */
/* to external numbering. The client stores "v[i] as "ext_v[map[i]".    */

extern void
  (* ExternalUnPrecond) (const int length,
       const int map [],
       NORMAL_PRECISION v []);
/* This function needs to be reassigned by a client application only if */
/* external preconditioning is used with the GMRES method.              */
/* "Q" being the preconditioner and "v" the unpreconditioned input      */
/* vector, this function should return in "v" the result of a           */
/* matrix-vector multiplication with the preconditioner, "Q v"          */
/* "map" is an index vector of length "length" that maps PILS numbering */
/* to external numbering. The client stores "v[i] as "ext_v[map[i]".    */

extern bool
  UsingExternalPreconditioning ();
/* Returns TRUE iff external preconditioning is  */
/* selected in the currently active solver type. */
  

/*----------------------------------------------------------*/

/* PILS interface #3:                                       */
/* =================                                        */

/* FORTRAN interface mimicking BLSMIP: one call matches all */

#define PadrePilsInterface fortran_name(pilsif,PILSIF)

extern void
  PadrePilsInterface (int *idc,
          int *nbk_Ptr,
          int *nub_Ptr,
          int *neqt_Ptr,
          int *nreg_Ptr,
          int *syflg0_Ptr,
          int *siter_Ptr,
          NORMAL_PRECISION *tol_Ptr,
          NORMAL_PRECISION *tola_Ptr,
          int *locar,
          int *ja,
          int *jr,
          NORMAL_PRECISION *alhs,
          NORMAL_PRECISION *rhs,
          NORMAL_PRECISION *x,
          int *ierr_Ptr);


#ifdef __cplusplus
}
#endif

#endif /* !PILSH */
