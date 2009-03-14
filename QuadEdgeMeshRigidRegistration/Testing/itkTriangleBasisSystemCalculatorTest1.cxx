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

#include "itkVector.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleBasisSystem.h"
#include "itkTriangleBasisSystemCalculator.h"
#include "itkTestingMacros.h"

int main(int argc, char *argv[])
{

  const unsigned int Dimension = 3;
  const unsigned int SurfaceDimension = 2; 

  typedef itk::QuadEdgeMesh< double, Dimension > MeshType;
  typedef MeshType::CellType CellType;
   
  MeshType::Pointer mesh = MeshType::New();

  typedef itk::Vector<double,3> VectorType; 
  VectorType v_01;
  VectorType v_02;

  typedef itk::TriangleBasisSystem< VectorType, SurfaceDimension>  TriangleBasisSystemType;
  TriangleBasisSystemType triangleBasisSystem;

  typedef itk::TriangleBasisSystemCalculator< MeshType, TriangleBasisSystemType >  TriangleBasisSystemCalculatorType;
  TriangleBasisSystemCalculatorType::Pointer triangleBasisSystemCalculator = TriangleBasisSystemCalculatorType::New(); 
  
  //Define a simple triangular cell
  typedef MeshType::PointType PointType; 
  PointType p0;
  PointType p1;
  PointType p2;
  p0[0]= 0.0; p0[1]= 0.0; p0[2]= 0.0;
  p1[0]= 1.0; p1[1]= 0.0; p1[2]= 0.0;
  p2[0]= 0.5; p2[1]= 1.0; p2[2]= 0.0;

  mesh->SetPoint( 0, p0 ); 
  mesh->SetPoint( 1, p1 ); 
  mesh->SetPoint( 2, p2 );

  //Should be SurfaceDimension-1 at most
  //Will compile later...
  //TRY_EXPECT_EXCEPTION( triangleBasisSystemCalculator->SetInputMesh(static_cast< MeshType > (NULL)) ); 
  //Should be SurfaceDimension-1 at most
  //TRY_EXPECT_EXCEPTION( triangleBasisSystemCalculator->GetInputMesh() ); 
  //Should be Dimension-1 at most
  //TRY_EXPECT_NO_EXCEPTION( triangleBasisSystemCalculator->SetInputMesh(mesh) ); 
  //Should be Dimension-1 at most
  //TRY_EXPECT_NO_EXCEPTION( triangleBasisSystemCalculator->GetInputMesh() ); 

  //FIXME need to include a basic test for CalculateTriangle...
  
  //std::cout << "Test passed." << std::endl;

  return EXIT_SUCCESS;
}
