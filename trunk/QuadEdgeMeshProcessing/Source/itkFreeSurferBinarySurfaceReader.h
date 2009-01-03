/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFreeSurferBinarySurfaceReader.h,v $
  Language:  C++
  Date:      $Date: 2008-01-15 19:10:40 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFreeSurferBinarySurfaceReader_h
#define __itkFreeSurferBinarySurfaceReader_h

#include "itkMesh.h"
#include "itkMeshSource.h"
#include "itkTriangleCell.h"
#include "itkMapContainer.h"
#include "itkIntTypes.h"

#include <fstream>

namespace itk
{

/** \class FreeSurferBinarySurfaceReader
 * \brief
 * Reads a surface from the FreeSurface binary file format.
 *
 * The description of the FreeSurface file format can be found at
 * http://wideman-one.com/gw/brain/fs/surfacefileformats.htm
 *
 */
template <class TOutputMesh>
class FreeSurferBinarySurfaceReader : public MeshSource<TOutputMesh>
{
public:
  /** Standard "Self" typedef. */
  typedef FreeSurferBinarySurfaceReader         Self;
  typedef MeshSource<TOutputMesh>               Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FreeSurferBinarySurfaceReader, MeshSource);

  /** Hold on to the type information specified by the template parameters. */
  typedef TOutputMesh                          OutputMeshType;
  typedef typename OutputMeshType::MeshTraits  MeshTraits;
  typedef typename OutputMeshType::PointType   PointType;
  typedef typename MeshTraits::PixelType       PixelType;

  /** Some convenient typedefs. */
  typedef typename OutputMeshType::Pointer         OutputMeshPointer;
  typedef typename OutputMeshType::CellTraits      CellTraits;
  typedef typename OutputMeshType::CellIdentifier  CellIdentifier;
  typedef typename OutputMeshType::CellType        CellType;
  typedef typename OutputMeshType::CellAutoPointer CellAutoPointer;
  typedef typename OutputMeshType::PointIdentifier PointIdentifier;
  typedef typename CellTraits::PointIdIterator     PointIdIterator;

  typedef typename OutputMeshType::PointsContainerPointer PointsContainerPointer;
  
  typedef typename OutputMeshType::PointsContainer PointsContainer;

  /** Define the triangular cell types which form the surface  */
  typedef TriangleCell<CellType>      TriangleCellType;

  typedef typename TriangleCellType::SelfAutoPointer TriangleCellAutoPointer;

  typedef std::pair<unsigned long,unsigned long>     IndexPairType;
  typedef MapContainer<IndexPairType, unsigned long> PointMapType;
  typedef typename PointType::VectorType             VectorType;

  /** Set the resolution level to be used for generating cells in the
   * Sphere. High values of this parameter will produce sphere with more
   * triangles. */
  /** Set/Get the name of the file to be read. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

protected:
  FreeSurferBinarySurfaceReader();
  ~FreeSurferBinarySurfaceReader() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Reads the file */
  void GenerateData();

  /** Filename to read */
  std::string      m_FileName;

private:
  FreeSurferBinarySurfaceReader(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  void OpenFile();
  void CloseFile();
  void ReadHeader();
  void ReadSurface();
  void ReadFileType();
  void ReadComment();
  void ReadNumberOfPoints();
  void ReadNumberOfCells();
  void ReadPoint();

  void ReadInteger32( ITK_UINT32 & valueToRead );
  void ReadFloat( float & valueToRead );

  std::ifstream  m_InputFile;

  ITK_UINT32     m_FileType;
  ITK_UINT32     m_NumberOfPoints;
  ITK_UINT32     m_NumberOfCells;
  std::string    m_Comment;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFreeSurferBinarySurfaceReader.txx"
#endif

#endif //_itkFreeSurferBinarySurfaceReader_h
