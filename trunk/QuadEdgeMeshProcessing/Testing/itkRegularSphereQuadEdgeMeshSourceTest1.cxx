/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRegularSphereQuadEdgeMeshSourceTest.cxx,v $
  Language:  C++
  Date:      $Date: 2007-09-06 17:44:24 $
  Version:   $Revision: 1.3 $

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
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

#include <iostream>

int main( int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << "Usage" << std::endl;
    std::cerr << argv[0] << " outputFileName.vtk" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::QuadEdgeMesh<float, 3>   MeshType;

  typedef itk::RegularSphereMeshSource< MeshType >  SphereMeshSourceType;

  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >   WriterType;

  SphereMeshSourceType::Pointer  mySphereMeshSource = SphereMeshSourceType::New();

  WriterType::Pointer writer = WriterType::New();

  typedef SphereMeshSourceType::PointType   PointType;
  typedef SphereMeshSourceType::VectorType  VectorType;

  PointType center; 
  center.Fill( 0.0 );

  VectorType scale;
  scale.Fill( 1.0 );
  
  mySphereMeshSource->SetCenter( center );
  mySphereMeshSource->SetResolution( 1 );
  mySphereMeshSource->SetScale( scale );

  mySphereMeshSource->Modified();

  writer->SetInput( mySphereMeshSource->GetOutput() );
  writer->SetFileName( argv[1] );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during Update() " << std::endl;
    std::cerr << excp << std::endl;
    }

  std::cout << "mySphereMeshSource: " << mySphereMeshSource;
  
  MeshType::Pointer myMesh = mySphereMeshSource->GetOutput();

  PointType  pt;

  std::cout << "Testing itk::RegularSphereMeshSource "<< std::endl;

  for(unsigned int i=0; i<myMesh->GetNumberOfPoints(); i++) 
    {
    myMesh->GetPoint(i, &pt);
    std::cout << "Point[" << i << "]: " << pt << std::endl;
    }

  std::cout << "Test End "<< std::endl;

  return EXIT_SUCCESS;

}
