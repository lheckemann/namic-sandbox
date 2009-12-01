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

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

// vnl headers
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_sparse_matrix.h>


namespace itk
{

/** \class LaplaceBeltramiFilter
 * \brief
 *
 * LaplaceBeltramiFilter defines basis functions on a quad edge mesh, then (if
 * requested) determines the N most significant eigenvalues of the basis.
 *
 * \ingroup MeshFilters
 * \sa TransformMeshFilter
 */

template <class TInputMesh, class TOutputMesh>
class ITK_EXPORT LaplaceBeltramiFilter :
    public QuadEdgeMeshToQuadEdgeMeshFilter<TInputMesh, TOutputMesh>
{
public:
  /** Standard class typedefs. */
  typedef LaplaceBeltramiFilter  Self;

  typedef TInputMesh   InputMeshType;
  typedef TOutputMesh  OutputMeshType;

  typedef QuadEdgeMeshToQuadEdgeMeshFilter<TInputMesh,TOutputMesh> Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef typename InputMeshType::ConstPointer  InputMeshConstPointer;
  typedef typename OutputMeshType::ConstPointer OutputMeshConstPointer;
  typedef typename InputMeshType::Pointer       InputMeshPointer;
  typedef typename OutputMeshType::Pointer      OutputMeshPointer;
  typedef typename InputMeshType::PointType     InputPointType;
  typedef typename OutputMeshType::PointType    OutputPointType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LaplaceBeltramiFilter, QuadEdgeMeshToQuadEdgeMeshFilter);

  void SetInput(TInputMesh *input);

  /** Convenient constants obtained from TMeshTraits template parameter. */
  itkStaticConstMacro(InputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TInputMesh >::PointDimension );
  itkStaticConstMacro(OutputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TOutputMesh >::PointDimension );

  typedef typename OutputMeshType::PointsContainer             OutputPointsContainer;
  typedef typename OutputMeshType::CellsContainer              OutputCellsContainer;
  typedef typename InputMeshType::PointsContainerPointer       PointsContainerPointer;
  typedef typename InputMeshType::CellsContainerPointer        CellsContainerPointer;
  typedef typename InputMeshType::CellsContainerConstPointer   InputCellsContainerConstPointer;
  typedef typename OutputMeshType::CellsContainerConstPointer  OutputCellsContainerConstPointer;
  typedef typename InputMeshType::PointIdentifier              InputPointIdentifier;
  typedef typename InputMeshType::CellIdentifier               InputCellIdentifier;
  typedef typename OutputPointsContainer::ConstPointer         OutputPointsContainerConstPointer;
  typedef typename OutputPointsContainer::ConstIterator        OutputPointIterator;
  typedef typename OutputCellsContainer::ConstIterator         OutputCellIterator;
  typedef typename InputMeshType::CellType                     InputCellType;
  typedef typename OutputMeshType::CellType                    OutputCellType;
  typedef typename InputCellType::PointIdIterator              InputPointIdIterator;
  typedef typename InputCellType::CellAutoPointer              InputCellAutoPointer;
  typedef typename OutputCellType::PointIdIterator             OutputPointIdIterator;
  typedef typename OutputCellType::CellAutoPointer             OutputCellAutoPointer;

  typedef typename OutputMeshType::PointDataContainer          OutputPointDataContainer;
  typedef typename OutputPointDataContainer::ConstPointer      OutputPointDataContainerConstPointer; 

  typedef vnl_sparse_matrix< double >  LBMatrixType;

  /** Set the number of eigenvalues to produce */
  void SetEigenValueCount( unsigned int );

  /** Get the Laplace Beltrami operator */
  void GetLBOperator( LBMatrixType& );

  /** Get the areas for each vertex */
  void GetVertexAreas( LBMatrixType& );

  /** Get a single surface harmonic */
  bool SetSurfaceHarmonic( unsigned int harmonic );

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

  /** Harmonics for the most significant basis functions */
  vnl_matrix<double> m_Harmonics;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLaplaceBeltramiFilter.txx"
#endif

#endif
