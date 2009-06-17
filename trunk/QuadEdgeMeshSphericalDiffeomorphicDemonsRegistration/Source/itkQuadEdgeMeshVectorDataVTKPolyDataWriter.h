/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshVectorDataVTKPolyDataWriter.h,v $
  Language:  C++
  Date:      $Date: 2009-01-02 18:43:05 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkQuadEdgeMeshVectorDataVTKPolyDataWriter_h
#define __itkQuadEdgeMeshVectorDataVTKPolyDataWriter_h

#include "itkVTKPolyDataWriter.h"
#include <fstream>

namespace itk
{
/**
 * \class QuadEdgeMeshScalarDataVTKPolyData
 *
 * \brief This class saves a QuadMesh into a VTK-legacy file format,
 *        including its vector data associated with points.
 *
 * \ingroup Writers
 *
 */
template< class TMesh >
class QuadEdgeMeshVectorDataVTKPolyDataWriter : public VTKPolyDataWriter< TMesh >
{
public:
  typedef QuadEdgeMeshVectorDataVTKPolyDataWriter         Self;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;
  typedef VTKPolyDataWriter< TMesh >                      Superclass;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( QuadEdgeMeshVectorDataVTKPolyDataWriter, VTKPolyDataWriter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TMesh                                           MeshType;
  typedef typename MeshType::Pointer                      MeshPointer;
  typedef typename MeshType::CellType                     CellType;

  typedef typename MeshType::PointsContainerPointer       PointsContainerPointer;
  typedef typename MeshType::PointsContainerIterator      PointsContainerIterator;

  typedef typename MeshType::PointDataContainerPointer          PointDataContainerPointer;
  typedef typename MeshType::PointDataContainerConstPointer     PointDataContainerConstPointer;
  typedef typename MeshType::PointDataContainerIterator         PointDataContainerIterator;

  typedef typename MeshType::CellsContainerPointer        CellsContainerPointer;
  typedef typename MeshType::CellsContainerIterator       CellsContainerIterator;

  typedef typename MeshType::CellDataContainerPointer     CellDataContainerPointer;
  typedef typename MeshType::CellDataContainerIterator    CellDataContainerIterator;
  
  /** Set/Get the name of the CellDataName where data are written. */
  itkSetStringMacro(CellDataName);
  itkGetStringMacro(CellDataName);

  /** Set/Get the name of the PointDataName where data are written. */
  itkSetStringMacro(PointDataName);
  itkGetStringMacro(PointDataName);

protected:
  QuadEdgeMeshVectorDataVTKPolyDataWriter();
  ~QuadEdgeMeshVectorDataVTKPolyDataWriter();

  std::string     m_CellDataName;
  std::string     m_PointDataName;

  void GenerateData();
  void WriteCellData();
  void WritePointData();

private:
  QuadEdgeMeshVectorDataVTKPolyDataWriter( const Self& );
  void operator = ( const Self& );
  
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshVectorDataVTKPolyDataWriter.txx"
#endif

#endif
