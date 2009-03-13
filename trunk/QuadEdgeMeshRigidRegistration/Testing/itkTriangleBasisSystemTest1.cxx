/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTriangleBasisSystemTest.cxx,v $
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

  typedef itk::TriangleBasisSystem<VectorType, SurfaceDimension>  TriangleBasisSystemType;
  TriangleBasisSystemType::Pointer triangleBasisSystem = TriangleBasisSystemType::New(); 
  
  //Define a simple triangular cell
  MeshType::PointType p0;
  MeshType::PointType p1;
  MeshType::PointType p2;
  p0[0]= 0.0; p0[1]= 0.0; p0[2]= 0.0;
  p1[0]= 1.0; p1[1]= 0.0; p1[2]= 0.0;
  p2[0]= 0.5; p2[1]= 1.0; p2[2]= 0.0;

  mesh->SetPoint( 0, p0 ); 
  mesh->SetPoint( 1, p1 ); 
  mesh->SetPoint( 2, p2 );


  for (int i=0; i<3; i++) {
     v_01[i]= p1[i]-p0[i];
     v_02[i]= p2[i]-p0[i];
  }

  //Should be SurfaceDimension-1 at most
  TRY_EXPECT_EXCEPTION( triangleBasisSystem->SetVector(SurfaceDimension, v_01) ); 
  //Should be Dimension-1 at most
  TRY_EXPECT_NO_EXCEPTION( triangleBasisSystem->SetVector(0, v_01) ); 
  //Should be Dimension-1 at most
  TRY_EXPECT_NO_EXCEPTION( triangleBasisSystem->SetVector(1, v_02) ); 

  //Should be Dimension-1 at most
  TRY_EXPECT_EXCEPTION( triangleBasisSystem->GetVector(SurfaceDimension) ); 

  std::cout << " basis vector 0 " <<
     triangleBasisSystem->GetVector(0) ;  
  std::cout << " basis vector 1 " <<
     triangleBasisSystem->GetVector(1) ; 
  
  std::cout << "Test passed." << std::endl;

  return EXIT_SUCCESS;
}
