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

template <class matrixDataType>
class theFunc : public vnl_cost_function {
 public:  
//   theFunc(vnl_matrix<matrixDataType> const& A, 
//           vnl_vector<matrixDataType> const& b);

  theFunc(vnl_sparse_matrix<matrixDataType> const& A, 
          vnl_vector<matrixDataType> const& b);

  double f(vnl_vector<matrixDataType> const& x);
  void gradf(vnl_vector<matrixDataType> const& x, vnl_vector<matrixDataType> & g);

  inline unsigned int dim() {return _dim;}

 private:
  vnl_matrix<matrixDataType> const* _A;
  vnl_sparse_matrix<matrixDataType> const* _Asparse;
  vnl_vector<matrixDataType> const* _b;
  unsigned int _dim;
  bool _sparse;
};



/* class linearEqnSolver { */
/*  public:   */
/*   linearEqnSolver(vnl_matrix<matrixDataType> const& A,  */
/*                   vnl_vector<matrixDataType> const& b); */

/*   linearEqnSolver(vnl_sparse_matrix<matrixDataType> const& A,  */
/*                   vnl_vector<matrixDataType> const& b); */

/*   vnl_vector<matrixDataType> solve(); */

/*  private: */
/*   theFunc _f; */
/*   vnl_conjugate_gradient _cg; */
/* }; */



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

  ///////////////////////
  typedef typename InputMeshType::PointsContainer::ConstIterator PointIterator;
  typedef typename InputMeshType::CellsContainer::ConstIterator CellIterator;  
  typedef typename InputMeshType::CellType CellType;
  typedef typename InputMeshType::CellType::PointIdIterator PointIdIterator;
  typedef typename InputMeshType::PointType PointType;

  typedef vnl_vector<CoordRepType> Tvnl_vector;

protected:
  ConformalFlatteningFilter();
  ~ConformalFlatteningFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Requested Data */
  virtual void GenerateData( void );

private:
  ConformalFlatteningFilter(const ConformalFlatteningFilter&); //purposely not implemented
  void operator=(const ConformalFlatteningFilter&); //purposely not implemented

  //  for conformal flattening mapping  
  //  theFunc<CoordRepType> _f;
  void mapping( InputMeshPointer inputMesh, OutputMeshPointer outputMesh);
  void getDb(OutputMeshPointer mesh, 
             vnl_sparse_matrix<CoordRepType> &D,
             vnl_vector<CoordRepType> &bR,
             vnl_vector<CoordRepType> &bI);  

//   vnl_vector<CoordRepType> solveLinearEq(vnl_matrix<CoordRepType> const& A, 
//                                    vnl_vector<CoordRepType> const& b);
  vnl_vector<CoordRepType> solveLinearEq(vnl_sparse_matrix<CoordRepType> const& A, 
                                   vnl_vector<CoordRepType> const& b);
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConformalFlatteningFilter.txx"
#endif

#endif
