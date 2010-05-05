/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshBoundarySmoothFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-09-08 20:00:56 $
  Version:   $Revision: 1.38 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkQuadEdgeMeshBoundarySmoothFilter_h
#define __itkQuadEdgeMeshBoundarySmoothFilter_h

#include <itkQuadEdgeMeshToQuadEdgeMeshFilter.h>


namespace itk
{
/**
 * \class QuadEdgeMeshBoundarySmoothFilter
 * \brief this filter takes two split surfaces,
 * smooth the boundaries of them, and gives two outputs
 * which have the smoothed boundaries.
 *
 * The filter is supposed to be used after itkQuadEdgeMeshSplitFilter
 * and before mapping the surface to sphere.

 * itkQuadEdgeMeshSplitFilter takes one surface as input and 
 * split it into two surfaces. There are "teeth" left on boundaries 
 * of splitted surfaces because of the triangulars on the surface.
 *
 * The smoothing method is applied in iterations until there is no
 * change happening in current iteration.
 *
 * Note: the total number of triangles on both inputs have to be
 * the same after each iteration. Missing cell or extra cell to 
 * the whole surface is not allowed.
 *
 * Inputs are sharing the same mesh type. So are the outputs.
 * \ingroup MeshFilters
 *
 */
  template< class TInputMesh, class TOutputMesh >
  class QuadEdgeMeshBoundarySmoothFilter : 
    public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
  {
  public:
    typedef QuadEdgeMeshBoundarySmoothFilter                            Self;
    typedef SmartPointer< Self >                                        Pointer;
    typedef SmartPointer< const Self >                                  ConstPointer;
    typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh > Superclass;
    
    /** Input types. */
    typedef TInputMesh                              InputMeshType;
    typedef typename InputMeshType::Pointer         InputMeshPointer;
    typedef typename InputMeshType::ConstPointer    InputMeshConstPointer;
    typedef typename InputMeshType::CoordRepType    InputCoordRepType;
    typedef typename InputMeshType::PointType       InputPointType;
    typedef typename InputMeshType::PointIdentifier InputPointIdentifier;
    typedef typename InputMeshType::QEType          InputQEType;
    typedef typename InputMeshType::QEPrimal        InputQEPrimal;
    typedef typename InputMeshType::VectorType      InputVectorType;
    typedef typename InputMeshType::CellType        InputCellType;
    typedef typename InputMeshType::CellIdentifier  InputCellIdentifier;

    typedef typename InputMeshType::PointsContainerConstPointer
      InputPointsContainerConstPointer;
    typedef typename InputMeshType::PointsContainerConstIterator
      InputPointsContainerConstIterator;
    typedef typename InputMeshType::CellsContainerConstPointer
      InputCellsContainerConstPointer;
    typedef typename InputMeshType::CellsContainerConstIterator
      InputCellsContainerConstIterator;

    typedef typename InputMeshType::EdgeCellType    InputEdgeCellType;
    typedef typename InputMeshType::PolygonCellType InputPolygonCellType;
    typedef typename InputMeshType::PointIdList     InputPointIdList;
    typedef typename InputMeshType::CellTraits      InputCellTraits;
    typedef typename InputCellTraits::PointIdInternalIterator
                                                    InputPointsIdInternalIterator;
//    typedef typename InputQEPrimal::IteratorGeom    InputQEIterator;

    
    /** Output types. */
    typedef TOutputMesh                               OutputMeshType;
    typedef typename OutputMeshType::Pointer          OutputMeshPointer;
    typedef typename OutputMeshType::ConstPointer     OutputMeshConstPointer;
    typedef typename OutputMeshType::CoordRepType     OutputCoordRepType;
    typedef typename OutputMeshType::PointType        OutputPointType;
    typedef typename OutputMeshType::PointIdentifier  OutputPointIdentifier;
    typedef typename OutputMeshType::PointIdList      OutputPointIdListType;

    typedef typename OutputMeshType::EdgeListPointerType  EdgeListPointerType;
    typedef typename OutputMeshType::QEPrimal             OutputQEPrimal;
    typedef typename OutputQEPrimal::IteratorGeom         OutputQEIterator;
    typedef typename OutputMeshType::PolygonCellType      OutputPolygonCellType;
    typedef typename OutputMeshType::PointsContainerPointer
      OutputPointsContainerPointer;
    typedef typename OutputMeshType::PointsContainerIterator
      OutputPointsContainerIterator;
    typedef typename OutputMeshType::CellsContainerConstPointer
      OutputCellsContainerConstPointer;
    typedef typename OutputMeshType::CellsContainerConstIterator
      OutputCellsContainerConstIterator;   
    
  public:
    itkNewMacro( Self );
    itkTypeMacro( QuadEdgeMeshBoundarySmoothFilter, QuadEdgeMeshToQuadEdgeMeshFilter );
    
    /** Set/Get the split0. */
    void SetInputMesh1 ( const InputMeshType * mesh1 );
    const InputMeshType * GetInputMesh1( void ) const;

    /** Set/Get the split1. */
    void SetInputMesh2 ( const InputMeshType * mesh2 );
    const InputMeshType * GetInputMesh2( void ) const;

    /** Get the smoothed split0. */
    OutputMeshType * GetOutputMesh1( void );

    /** Get the smoothed split1. */
    OutputMeshType * GetOutputMesh2( void );

    /** Set the number of iterations. */
    itkSetMacro( Iterations, int );
    /** Get the number of iterations. */
    itkGetMacro( Iterations, int );

  protected:
    QuadEdgeMeshBoundarySmoothFilter();
    ~QuadEdgeMeshBoundarySmoothFilter();
    
    void CopyInputMeshesToOutputMeshes();

    int AdjustBoundary( OutputMeshType * deleteMesh, OutputMeshType * addMesh);

    virtual void GenerateData( );

    
  private:
    QuadEdgeMeshBoundarySmoothFilter( const Self& );
    void operator = ( const Self& );

    int m_Iterations;
  };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshBoundarySmoothFilter.txx"
#endif

#endif
