/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFreeSurferBinarySurfaceReader.txx,v $
  Language:  C++
  Date:      $Date: 2008-06-15 02:42:12 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFreeSurferBinarySurfaceReader_txx
#define __itkFreeSurferBinarySurfaceReader_txx

#include "itkFreeSurferBinarySurfaceReader.h"
#include "itkByteSwapper.h"

namespace itk
{

// 
// Constructor
// 
template<class TOutputMesh>
FreeSurferBinarySurfaceReader<TOutputMesh>
::FreeSurferBinarySurfaceReader()
{
  //
  // Create the output
  //
  typename TOutputMesh::Pointer output = TOutputMesh::New();
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
}

template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::GenerateData()
{
  this->OpenFile();
  this->ReadHeader();
  this->PrepareOutputMesh();
  this->ReadSurface();
  this->CloseFile();
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::OpenFile()
{
  if( this->m_FileName == "" )
    {
    itkExceptionMacro("No input FileName");
    }

  //
  // Open file 
  //
  this->m_InputFile.open( this->m_FileName.c_str() );

  if( !this->m_InputFile.is_open() )
    {
    itkExceptionMacro("Unable to open file\n"
        "inputFilename= " << this->m_FileName );
    }
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::CloseFile()
{
  this->m_InputFile.close();
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadHeader()
{
  this->ReadFileType();
  this->ReadComment();
  this->ReadNumberOfPoints();
  this->ReadNumberOfCells();
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadFileType()
{
  const unsigned int fileTypeIdLength = 3;
  char fileTypeId[fileTypeIdLength];

  this->m_InputFile.read( fileTypeId, fileTypeIdLength );

  this->m_FileType = 0;
  this->m_FileType <<= 8;

  this->m_FileType |= fileTypeId[0];
  this->m_FileType <<= 8;

  this->m_FileType |= fileTypeId[1];
  this->m_FileType <<= 8;

  this->m_FileType |= fileTypeId[2];
  this->m_FileType <<= 8;

  itk::ByteSwapper<ITK_UINT32>::SwapFromSystemToBigEndian( &(this->m_FileType) );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadComment()
{
  char byte;

  //
  //  Extract Comment, and ignore it.
  //
  byte = this->m_InputFile.get();

  this->m_Comment = "";

  while( byte != '\n' )
    {
    this->m_Comment += byte;
    byte = this->m_InputFile.get();
    }


  //
  // Try to get the second '\n', but if the '\n' is not there, we put the byte
  // back.
  //
  byte = this->m_InputFile.get();
  if( byte != '\n' )
    {
    this->m_InputFile.unget();
    }
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadNumberOfPoints()
{
  this->ReadInteger32( this->m_NumberOfPoints );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadNumberOfCells()
{
  this->ReadInteger32( this->m_NumberOfCells );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::PrepareOutputMesh()
{
  typename OutputMeshType::Pointer outputMesh = this->GetOutput();

  outputMesh->SetCellsAllocationMethod(
      OutputMeshType::CellsAllocatedDynamicallyCellByCell );

  outputMesh->GetPoints()->Reserve( this->m_NumberOfPoints );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadSurface()
{
  this->ReadPoints();
  this->ReadCells();
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadPoints()
{
  typename OutputMeshType::Pointer outputMesh = this->GetOutput();

  PointType point;
  for( unsigned int ip=0; ip < this->m_NumberOfPoints; ip++ )
    {
    this->ReadPoint( point );
    outputMesh->SetPoint( ip, point );
    }
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadCells()
{
  typename OutputMeshType::Pointer outputMesh = this->GetOutput();

  for( unsigned int ic=0; ic < this->m_NumberOfCells; ic++ )
    {
    TriangleCellType * triangleCell = new TriangleCellType;

    this->ReadCell( *triangleCell );

    CellAutoPointer cell;
    cell.TakeOwnership( triangleCell );
    outputMesh->SetCell( ic, cell );
    }
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadCell( TriangleCellType & triangleCell )
{
  const unsigned int numberOfCellPoints = 3; // Triangles

  ITK_UINT32 pointId;

  for( PointIdentifier k = 0; k < numberOfCellPoints; k++ )
    {
    this->ReadInteger32( pointId );
    triangleCell.SetPointId( k, pointId );
    }
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadInteger32( ITK_UINT32 & valueToRead )
{
  this->m_InputFile.read( (char *)(&valueToRead), sizeof(valueToRead) );

  itk::ByteSwapper<ITK_UINT32>::SwapFromSystemToBigEndian( &valueToRead );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadFloat( float & valueToRead  )
{
  this->m_InputFile.read( (char *)(&valueToRead), sizeof(valueToRead) );

  itk::ByteSwapper<float>::SwapFromSystemToBigEndian( &valueToRead );
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadPoint( PointType & point )
{
  float x;
  float y;
  float z;
  this->ReadFloat( x );
  this->ReadFloat( y );
  this->ReadFloat( z );
  
  point[0] = x;
  point[1] = y;
  point[2] = z;
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " << this->m_FileName << std::endl;
  os << indent << "FileType: " << this->m_FileType << std::endl;
  os << indent << "Comment : " << this->m_Comment << std::endl;
  os << indent << "Number of Points : " << this->m_NumberOfPoints << std::endl;
  os << indent << "Number of Cells  : " << this->m_NumberOfCells << std::endl;
}

} //end of namespace itk


#endif
