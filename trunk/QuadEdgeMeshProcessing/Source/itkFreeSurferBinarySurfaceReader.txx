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

  std::cout << "FileType " 
    << int(fileTypeId[0]) << " "
    << int(fileTypeId[1]) << " "
    << int(fileTypeId[2]) << " " << std::endl;

  itk::ByteSwapper<ITK_UINT32>::SwapFromSystemToBigEndian( &(this->m_FileType) );

  std::cout << this->m_FileType << std::endl;
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

  std::cout << "Comment = " << this->m_Comment << std::endl;

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
  std::cout << "Number of points = " << this->m_NumberOfPoints << std::endl;
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::ReadNumberOfCells()
{
  this->ReadInteger32( this->m_NumberOfCells );
  std::cout << "Number of cells = " << this->m_NumberOfCells << std::endl;
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
  std::cout <<  "Point = " << x << " " << y << " " << z << std::endl;
  
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
}

} //end of namespace itk


#endif
