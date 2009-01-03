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
#include <fstream>
#include <stdio.h>

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

  //
  // Open file 
  //
  std::ifstream inputFile;

  inputFile.open( m_FileName.c_str() );

  if( !inputFile.is_open() )
    {
    itkExceptionMacro("Unable to open file\n"
        "inputFilename= " << m_FileName );
    }

  //
  //  Read Header
  //
  const unsigned int fileTypeIdLength = 3;
  char fileTypeId[fileTypeIdLength];
  inputFile.read( fileTypeId, fileTypeIdLength );

  std::cout << "FileType " 
    << int(fileTypeId[0]) << " "
    << int(fileTypeId[1]) << " "
    << int(fileTypeId[1]) << " " << std::endl;

  inputFile.close();
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
