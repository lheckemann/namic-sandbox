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
#include "itkVTKPolyDataReader.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkLinearInterpolateMeshFunction.h"

#include <iostream>

int main(int argc, char* argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: itkFreeSurferBinarySurfaceReaderTest inputFilename";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::QuadEdgeMesh<float, 3>         MeshType;
  typedef itk::VTKPolyDataReader< MeshType >  ReaderType;

  typedef MeshType::PointsContainer    PointsContainer;

  ReaderType::Pointer  surfaceReader = ReaderType::New();

  typedef ReaderType::PointType   PointType;

  surfaceReader->SetFileName(argv[1]);

  try
    {
    surfaceReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during Update() " << std::endl;
    std::cerr << excp << std::endl;
    }


  MeshType::Pointer mesh = surfaceReader->GetOutput();

  typedef itk::LinearInterpolateMeshFunction< MeshType >   InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( mesh );

  interpolator->Initialize();

  PointsContainer::Pointer  points = mesh->GetPoints();

  PointsContainer::ConstIterator  pointItr = points->Begin();
  PointsContainer::ConstIterator  pointEnd = points->End();

  PointType  point;

  InterpolatorType::InstanceIdentifierVectorType  pointIds(3);

  PointsContainer::ElementIdentifier pointId = 0;

  while( pointItr != pointEnd )
    {
    interpolator->FindTriangle( pointItr.Value(), pointIds );

    if( pointId != pointIds[0]  && pointId != pointIds[1]  && pointId != pointIds[2] )
      {
      std::cerr << "The triangle found doesn't has the point as vertex" << std::endl;
      std::cerr << "pointId " << pointId << std::endl;
      std::cerr << "pointIds[0] " << pointIds[0] << std::endl;
      std::cerr << "pointIds[1] " << pointIds[1] << std::endl;
      std::cerr << "pointIds[2] " << pointIds[2] << std::endl;
      return EXIT_FAILURE;
      }

    ++pointItr;
    ++pointId;
    }

  std::cout << "Checked " << pointId << " points " << std::endl;
  std::cout << "from a mesh with " << mesh->GetNumberOfPoints() << " points " << std::endl;

  std::cout << "Test passed"<< std::endl;

  return EXIT_SUCCESS;
}
