/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFreeSurferBinarySurfaceReaderQuadEdgeMeshTest.cxx,v $
  Language:  C++
  Date:      $Date: 2008-06-16 02:07:17 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif


#include "itkQuadEdgeMesh.h"
#include "itkFreeSurferBinarySurfaceReader.h"
#include "itkDefaultStaticMeshTraits.h"

#include <iostream>

int main(int argc, char* argv[] )
{
  if( argc != 2 )
    {
    std::cerr << "Usage: itkFreeSurferBinarySurfaceReaderTest inputFilename"
      << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::QuadEdgeMesh<float, 3>         MeshType;
  typedef itk::FreeSurferBinarySurfaceReader< MeshType >  ReaderType;

  ReaderType::Pointer  polyDataReader = ReaderType::New();

  typedef ReaderType::PointType   PointType;
  typedef ReaderType::VectorType  VectorType;

  polyDataReader->SetFileName(argv[1]);

  try
    {
    polyDataReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during Update() " << std::endl;
    std::cerr << excp << std::endl;
    }

  std::cout << "polyDataReader:" << std::endl;
  std::cout << polyDataReader << std::endl;
 
  MeshType::Pointer mesh = polyDataReader->GetOutput();

  std::cout << "Using following MeshType :";
  std::cout << mesh->GetNameOfClass( ) << std::endl; 
  
  PointType  point;

  std::cout << "Testing itk::FreeSurferBinarySurfaceReader" << std::endl;

  unsigned int numberOfPoints = mesh->GetNumberOfPoints();
  unsigned int numberOfCells  = mesh->GetNumberOfCells();

  std::cout << "numberOfPoints= " << numberOfPoints << std::endl;
  std::cout << "numberOfCells= " << numberOfCells << std::endl;

  if( !numberOfPoints )
    {
    std::cerr << "ERROR: numberOfPoints= " << numberOfPoints << std::endl;
    return EXIT_FAILURE;
    }

  if( !numberOfCells )
    {
    std::cerr << "ERROR: numberOfCells= " << numberOfCells << std::endl;
    return EXIT_FAILURE;
    }

  for(unsigned int i=0; i<numberOfPoints; i++)
    {
    // mesh->GetPoint(i, &point);
    //std::cout << "Point[" << i << "]: " << point << std::endl;
    }

  std::cout << "Test passed"<< std::endl;
  return EXIT_SUCCESS;
}
