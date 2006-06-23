/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkConformalFlatteningFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:58 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkConformalFlatteningFilter_h
#define __itkConformalFlatteningFilter_h

#include "itkMeshToMeshFilter.h"

#include <assert.h>
#include <vector>

// vnl headers
#include <vcl_iostream.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/algo/vnl_conjugate_gradient.h>

#ifndef matrixDataType
#define matrixDataType double
#endif


class theFunc : public vnl_cost_function {
 public:  
  theFunc(vnl_matrix<double> const& A, 
          vnl_vector<double> const& b);

  theFunc(vnl_sparse_matrix<double> const& A, 
          vnl_vector<double> const& b);

  double f(vnl_vector<double> const& x);
  void gradf(vnl_vector<double> const& x, vnl_vector<double> & g);

  inline unsigned int dim() {return _dim;}

 private:
  vnl_matrix<double> const* _A;
  vnl_sparse_matrix<double> const* _Asparse;
  vnl_vector<double> const* _b;
  unsigned int _dim;
  bool _sparse;
};



class linearEqnSolver {
 public:  
  linearEqnSolver(vnl_matrix<double> const& A, 
                  vnl_vector<double> const& b);

  linearEqnSolver(vnl_sparse_matrix<double> const& A, 
                  vnl_vector<double> const& b);

  vnl_vector<double> solve();

 private:
  theFunc _f;
  vnl_conjugate_gradient _cg;
};



namespace itk
{

/** \class ConformalFlatteningFilter
 * \brief 
 *
 * 
 * \ingroup MeshFilters
 */
template <class TInputMesh, class TOutputMesh>
class ITK_EXPORT ConformalFlatteningFilter : 
    public MeshToMeshFilter<TInputMesh,TOutputMesh>
{
public:
  /** Standard class typedefs. */
  typedef ConformalFlatteningFilter  Self;
  typedef MeshToMeshFilter<TInputMesh,TOutputMesh> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  typedef TInputMesh InputMeshType;
  typedef TOutputMesh OutputMeshType;
  typedef typename InputMeshType::Pointer InputMeshPointer;
  typedef typename OutputMeshType::Pointer OutputMeshPointer;
  
  /** Type for representing coordinates. */
  typedef typename TInputMesh::CoordRepType  CoordRepType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ConformalFlatteningFilter, MeshToMeshFilter);

protected:
  ConformalFlatteningFilter();
  ~ConformalFlatteningFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Requested Data */
  virtual void GenerateData( void );

private:
  ConformalFlatteningFilter(const ConformalFlatteningFilter&); //purposely not implemented
  void operator=(const ConformalFlatteningFilter&); //purposely not implemented

  // functions for conformal flattening mapping  
  void mapping( OutputMeshPointer mesh);
  void getDb(OutputMeshPointer mesh, 
             vnl_sparse_matrix<matrixDataType> &D,
             vnl_vector<matrixDataType> &bR,
             vnl_vector<matrixDataType> &bI);  
};

} // end namespace itk

/* #ifndef ITK_MANUAL_INSTANTIATION */
/* #include "itkConformalFlatteningFilter.txx" */
/* #endif */

#endif
