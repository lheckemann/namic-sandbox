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

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"
#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkMeshGeneratorHelper.h"

int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedMeshFile  movingMeshFile ";
    std::cerr << " outputMeshfile " << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef float      MeshPixelType;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   FixedMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   MovingMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   RegisteredMeshType;

  typedef itk::QuadEdgeMeshSphericalDiffeomorphicDemonsFilter<
    FixedMeshType, MovingMeshType, RegisteredMeshType >   DemonsFilterType;

  DemonsFilterType::Pointer demonsFilter = DemonsFilterType::New();

  std::cout << demonsFilter->GetNameOfClass() << std::endl;
  demonsFilter->Print( std::cout );

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

  demonsFilter->SetFixedMesh( fixedReader->GetOutput() );
  demonsFilter->SetMovingMesh( movingReader->GetOutput() );

  if( demonsFilter->GetFixedMesh() != fixedReader->GetOutput() )
    {
    std::cerr << "Error in SetFixedMesh()/GetFixedMesh() " << std::endl;
    return EXIT_FAILURE;
    }

  if( demonsFilter->GetMovingMesh() != movingReader->GetOutput() )
    {
    std::cerr << "Error in SetMovingMesh()/GetMovingMesh() " << std::endl;
    return EXIT_FAILURE;
    }


  try
    {
    demonsFilter->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while running the Demons filter " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  itk::MeshWriterHelper<RegisteredMeshType>::WriteMeshToFile( demonsFilter->GetOutput(), argv[3] );

  typedef DemonsFilterType::DestinationPointContainerType    DestinationPointContainerType;
  typedef DestinationPointContainerType::ConstPointer        DestinationPointContainerConstPointer;

  DestinationPointContainerConstPointer destinationPoints = demonsFilter->GetDestinationPoints();

  std::cout << "Number of Destination points = " << destinationPoints->Size() << std::endl;

  typedef DemonsFilterType::BasisSystemContainerType         BasisSystemContainerType;
  typedef BasisSystemContainerType::ConstPointer             BasisSystemContainerPointer;

  BasisSystemContainerPointer basisSystems = demonsFilter->GetBasisSystemAtNode();

  std::cout << "Number of basis systems = " << basisSystems->Size() << std::endl;

  return EXIT_SUCCESS;
}
