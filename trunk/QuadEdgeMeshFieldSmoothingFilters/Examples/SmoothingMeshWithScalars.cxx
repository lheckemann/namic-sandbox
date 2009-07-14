/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ImageRegistration8.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-10 19:46:31 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadEdgeMeshScalarPixelValuesSmoothingFilter.h"
#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkQuadEdgeMeshVectorDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMesh.h"

int main( int argc, char *argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputMeshFile  outputMeshfile lambda iterations" << std::endl;
    return EXIT_FAILURE;
    }
  
  const unsigned int Dimension = 3;
  typedef float      MeshPixelType;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   InputMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   OutputMeshType;

  typedef itk::QuadEdgeMeshScalarPixelValuesSmoothingFilter<
    InputMeshType, OutputMeshType >                       FilterType;

  FilterType::Pointer filter = FilterType::New();

  typedef itk::QuadEdgeMeshVTKPolyDataReader< InputMeshType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetInput( reader->GetOutput() );

  filter->SetLambda( atof( argv[3] ) );
  filter->SetMaximumNumberOfIterations( atoi( argv[4] ) );

  return EXIT_SUCCESS;
}
