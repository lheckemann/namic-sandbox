/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNodeScalarGradientCalculatorTest1.cxx,v $
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

#include "itkNodeScalarGradientCalculator.h"
#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkMeshGeneratorHelper.h"
#include "itkCovariantVector.h"
#include "itkTestingMacros.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleBasisSystemCalculator.h"
#include "itkTriangleListBasisSystemCalculator.h"

int main( int argc, char *argv[] )
{
  
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedMeshFile  movingMeshFile " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef float      MeshPixelType;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   FixedMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   MovingMeshType;

  typedef MovingMeshType::PointDataContainer              MovingPointDataContainerType;

  typedef itk::NodeScalarGradientCalculator< 
    FixedMeshType, MovingPointDataContainerType  >   GradientCalculatorType;

  GradientCalculatorType::Pointer gradientCalculator = GradientCalculatorType::New();

  std::cout << gradientCalculator->GetNameOfClass() << std::endl;
  gradientCalculator->Print( std::cout );

  typedef itk::VTKPolyDataReader< FixedMeshType >         FixedReaderType;
  typedef itk::VTKPolyDataReader< MovingMeshType >        MovingReaderType;

  FixedReaderType::Pointer fixedReader = FixedReaderType::New();
  fixedReader->SetFileName( argv[1] );

  MovingReaderType::Pointer movingReader = MovingReaderType::New();
  movingReader->SetFileName( argv[2] );

  try
    {
    fixedReader->Update( );
    movingReader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "test 4 \n";


  //Have not properly initialized gradientCalculator yet...
  TRY_EXPECT_EXCEPTION( gradientCalculator->Compute(); );
  //First item needed: triangle basis list.
  const unsigned int SurfaceDimension = 2;
  typedef FixedMeshType::PointType  PointType;
  typedef PointType::VectorType     VectorType;
  typedef itk::TriangleBasisSystem< VectorType, SurfaceDimension>  TriangleBasisSystemType;
  TriangleBasisSystemType triangleBasisSystem;

  typedef itk::TriangleListBasisSystemCalculator< FixedMeshType, TriangleBasisSystemType >
     TriangleListBasisSystemCalculatorType;

  TriangleListBasisSystemCalculatorType::Pointer triangleListBasisSystemCalculator =
     TriangleListBasisSystemCalculatorType::New();

  triangleListBasisSystemCalculator->SetInputMesh( fixedReader->GetOutput() );

  //Should be able to compute basis list with fixed mesh.
  TRY_EXPECT_NO_EXCEPTION( triangleListBasisSystemCalculator->Calculate() );

  std::cout << "test 3 \n";

  //Still have not properly initialized gradientCalculator yet... 2 to go
  TRY_EXPECT_EXCEPTION( gradientCalculator->Compute(); );

  gradientCalculator->SetInputMesh( triangleListBasisSystemCalculator->GetInputMesh() );

  if( gradientCalculator->GetInputMesh() != triangleListBasisSystemCalculator->GetInputMesh() )
    {
    std::cerr << "Error in SetFixedMesh()/GetFixedMesh() " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "test 2 \n";

  //Still have not properly initialized gradientCalculator yet... 1 to go
  TRY_EXPECT_EXCEPTION( gradientCalculator->Compute(); );

  MovingPointDataContainerType::Pointer movingData = MovingPointDataContainerType::New();
  movingData->Reserve( fixedReader->GetOutput()->GetNumberOfPoints() );

  MovingPointDataContainerType::Iterator dataItr = movingData->Begin();
  MovingPointDataContainerType::Iterator dataEnd = movingData->End();

  while( dataItr != dataEnd )
    {
    dataItr.Value() = 1.0;
    ++dataItr;
    }

  std::cout << "GetPointData from moving mesh " << movingData.GetPointer() << std::endl;

  gradientCalculator->SetDataContainer( movingData );

  if( gradientCalculator->GetDataContainer() != movingData )
    {
    std::cerr << "Error in SetDataContainer()/GetDataContainer() " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "test 1 gradientCalculator->GetDataContainer() " <<
     gradientCalculator->GetDataContainer() << "\n";
  
  //Have still not properly initialized gradientCalculator yet...
  TRY_EXPECT_EXCEPTION( gradientCalculator->Compute(); );

  gradientCalculator->SetBasisSystemList( triangleListBasisSystemCalculator->GetBasisSystemList() );

  if( gradientCalculator->GetBasisSystemList() != triangleListBasisSystemCalculator->GetBasisSystemList() )
    {
    std::cerr << "Error in SetBasisSystemList()/GetBasisSystemList() " << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "test 1 gradientCalculator->GetDataContainer() " <<
     gradientCalculator->GetDataContainer() << "\n";
  

  // It is not initialized correctly, we expect no exception
  TRY_EXPECT_NO_EXCEPTION( gradientCalculator->Compute(); );

  try
    {
    gradientCalculator->Evaluate( 17 );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
