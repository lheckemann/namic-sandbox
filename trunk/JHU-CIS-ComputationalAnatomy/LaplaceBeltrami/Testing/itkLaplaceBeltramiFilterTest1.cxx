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

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshVTKPolyDataReader.h"
#include "itkLaplaceBeltramiFilter.h"

int main( int argc, char *argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedMeshFile  movingMeshFile " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef float      MeshPixelType1;
  typedef double      MeshPixelType2;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType1, Dimension >   InMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType2, Dimension >   OutMeshType;

  typedef itk::QuadEdgeMeshVTKPolyDataReader< InMeshType >   ReaderType;

  typedef itk::LaplaceBeltramiFilter< InMeshType, OutMeshType >   FilterType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );

  reader->Update();


  FilterType::Pointer filter = FilterType::New();

  std::cout << "Name of Class = " << filter->GetNameOfClass() << std::endl;

  std::cout << "Test Print() = " << std::endl;
  filter->Print( std::cout );

  // Intentionally run it without an input to test error management
  try
    {
    filter->Update();
    std::cerr << "Failed to throw expected exception" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << "SUCCESFULLY catched expected exception" << std::endl;
    std::cout << excp << std::endl;
    }

  // Purposely attempt to set a harmonic that is too large
  filter->SetSurfaceHarmonic( 10000 );

  // Now setting it to an acceptable value
  filter->SetSurfaceHarmonic( 0 );

  //
  //  Now connect the input and verify that it runs fine.
  // 
  filter->SetInput( reader->GetOutput() );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  // Now run the filter without computing eigen values.
  filter->SetEigenValueCount( 0 );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
