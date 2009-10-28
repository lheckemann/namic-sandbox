/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceBeltramiFilter.h,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceBeltramiFilter_h
#define __itkLaplaceBeltramiFilter_h

#include "itkMesh.h"
#include "itkMeshToMeshFilter.h"

// vnl headers
#include <vnl/vnl_math.h>
#include <vnl/vnl_sparse_matrix.h>


namespace itk
{

/** \class LaplaceBeltramiFilter
 * \brief
 *
 * LaplaceBeltramiFilter defines basis functions on a mesh, then (if
 * requested) determines the N most significant eigenvalues of the basis.
 *
 * \ingroup MeshFilters
 * \sa TransformMeshFilter
 */

template <class TInputMesh, class TOutputMesh, typename TCompRep = float>
class ITK_EXPORT LaplaceBeltramiFilter :
    public MeshToMeshFilter<TInputMesh, TOutputMesh>
{
public:
  /** Standard class typedefs. */
  typedef LaplaceBeltramiFilter  Self;

  typedef TInputMesh   InputMeshType;
  typedef TOutputMesh  OutputMeshType;

  typedef MeshToMeshFilter<TInputMesh,TOutputMesh> Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef typename InputMeshType::Pointer      InputMeshPointer;
  typedef typename OutputMeshType::Pointer     OutputMeshPointer;
  typedef typename InputMeshType::PointType    InputPointType;
  typedef typename OutputMeshType::PointType   OutputPointType;

  /** Type for representing coordinates. */
  //typedef typename TInputMesh::CoordRepType          CoordRepType;
  typedef double CoordRepType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LaplaceBeltramiFilter,MeshToMeshFilter);

  void SetInput(TInputMesh *input);

  /** Convenient constants obtained from TMeshTraits template parameter. */
  itkStaticConstMacro(InputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TInputMesh >::PointDimension );
  itkStaticConstMacro(OutputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TOutputMesh >::PointDimension );

  typedef typename InputMeshType::PointsContainer    PointsContainer;
  typedef typename InputMeshType::CellsContainer     CellsContainer;
  typedef typename InputMeshType::PointIdentifier    PointIdentifier;
  typedef typename InputMeshType::CellIdentifier     CellIdentifier;
  typedef typename PointsContainer::ConstIterator    PointIterator;
  typedef typename CellsContainer::ConstIterator     CellIterator;
  typedef typename InputMeshType::CellType           CellType;
  typedef typename CellType::PointIdIterator         PointIdIterator;
  typedef typename CellType::CellAutoPointer         CellAutoPointer;

  typedef vnl_sparse_matrix< TCompRep >  LBMatrixType;

  /** Set the number of eigenvalues to produce */
  void SetEigenValueCount( unsigned int );

  /** Get the Laplace Beltrami operator */
  void GetLBOperator( LBMatrixType& );

  /** Get the areas for each vertex */
  void GetVertexAreas( LBMatrixType& );

protected:
  LaplaceBeltramiFilter();
  ~LaplaceBeltramiFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Generate Requested Data */
  virtual void GenerateData( void );

private:
  //purposely not implemented
  LaplaceBeltramiFilter(const LaplaceBeltramiFilter&);
  //purposely not implemented
  void operator=(const LaplaceBeltramiFilter&);

  /** Number of most significant eigenvalues to include */
  unsigned int m_EigenValueCount;

  /** The LB operator */
  LBMatrixType m_LBOperator;

  /** The areas for each vertex */
  LBMatrixType m_VertexAreas;

  /** Harmonics for the most significan basis functions */
  vnl_matrix<TCompRep> m_Harmonics;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLaplaceBeltramiFilter.txx"
#endif

#endif
