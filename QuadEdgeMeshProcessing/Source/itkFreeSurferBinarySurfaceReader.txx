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
  typename OutputMeshType::Pointer outputMesh = this->GetOutput();

  outputMesh->SetCellsAllocationMethod(
      OutputMeshType::CellsAllocatedDynamicallyCellByCell );

  if( m_FileName == "" )
    {
    itkExceptionMacro("No input FileName");
    }

  this->OpenFile();
  this->ReadHeader();
  this->ReadSurface();
  this->CloseFile();
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::OpenFile()
{
  //
  // Open file 
  //
  this->m_InputFile.open( m_FileName.c_str() );

  if( !this->m_InputFile.is_open() )
    {
    itkExceptionMacro("Unable to open file\n"
        "inputFilename= " << m_FileName );
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
::ReadSurface()
{
  for( unsigned int ip=0; ip < this->m_NumberOfPoints; ip++ )
    {
    this->ReadPoint();
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
::ReadPoint()
{
  float x;
  float y;
  float z;
  this->ReadFloat( x );
  this->ReadFloat( y );
  this->ReadFloat( z );
  std::cout <<  "Point = " << x << " " << y << " " << z << std::endl;
}


template<class TOutputMesh>
void
FreeSurferBinarySurfaceReader<TOutputMesh>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "FileName: " << m_FileName << std::endl;
}

} //end of namespace itk


#endif
