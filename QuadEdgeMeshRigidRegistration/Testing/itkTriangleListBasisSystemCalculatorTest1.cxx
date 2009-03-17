/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystemCalculatorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005-07-26 15:55:12 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkPoint.h"
#include "itkVector.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleBasisSystemCalculator.h"
#include "itkTriangleListBasisSystemCalculator.h"
#include "itkRegularSphereMeshSource.h"
#include "itkMeshGeneratorHelper.h"
#include "itkTestingMacros.h"

int main(int argc, char *argv[])
{

  const unsigned int SurfaceDimension = 2;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< float, Dimension >   MovingMeshType;
  typedef itk::QuadEdgeMesh< float, Dimension >   MeshType;

  MeshType::Pointer   mesh;
  MovingMeshType::Pointer  movingMesh;
  typedef MeshType::PointType       PointType;
  typedef PointType::VectorType     VectorType;

  typedef MeshGeneratorHelper< MeshType, MovingMeshType >  GeneratorType;

  GeneratorType::GenerateMeshes( mesh, movingMesh );

  typedef itk::TriangleBasisSystem< VectorType, SurfaceDimension>  TriangleBasisSystemType;
  TriangleBasisSystemType triangleBasisSystem;

  typedef itk::TriangleListBasisSystemCalculator< MeshType, TriangleBasisSystemType >
     TriangleListBasisSystemCalculatorType;

  TriangleListBasisSystemCalculatorType::Pointer triangleListBasisSystemCalculator =
     TriangleListBasisSystemCalculatorType::New();

  std::cout << triangleListBasisSystemCalculator->GetNameOfClass() << std::endl;
  triangleListBasisSystemCalculator->Print( std::cout );

  //Mesh not set yet. Exception!
  TRY_EXPECT_EXCEPTION( triangleListBasisSystemCalculator->Calculate() );

  TRY_EXPECT_NO_EXCEPTION( triangleListBasisSystemCalculator->SetInputMesh( mesh ) );

  //List not computed yet. Exception!
  TRY_EXPECT_EXCEPTION( triangleListBasisSystemCalculator->GetBasisSystemList() );

  TEST_SET_GET( mesh, triangleListBasisSystemCalculator->GetInputMesh() );

  //This should produce a list
  TRY_EXPECT_NO_EXCEPTION( triangleListBasisSystemCalculator->Calculate() );

  const TriangleListBasisSystemCalculatorType::BasisSystemListType * basisSystemListPtr = 
    triangleListBasisSystemCalculator->GetBasisSystemList();

  TriangleListBasisSystemCalculatorType::BasisSystemListIterator basisSystemListIterator;

  basisSystemListIterator = basisSystemListPtr->Begin();

  std::cout << " basis list first element " << std::endl;
  std::cout << basisSystemListIterator->Value().GetVector(0) << std::endl;
  std::cout << basisSystemListIterator->Value().GetVector(1) << std::endl;

  std::cout << "Test passed." << std::endl;

  return EXIT_SUCCESS;
}
