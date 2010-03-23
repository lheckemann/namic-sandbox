/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanSquaresMeshToMeshMetricTest1.cxx,v $
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

#include "itkQuadEdgeMeshSphericalDiffeomorphicDemonsRegistrationConfigure.h"

#include "itkCommand.h"
#include "itkVTKPolyDataReader.h"

#include "itkMultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter.h"

#ifdef USE_VTK
#include "DeformableAndAffineRegistrationMonitor.h"
#include "vtkSmartPointer.h"
#include "vtkPolyDataReader.h"
#endif


int main( int argc, char * argv [] )
{

  if( argc < 17 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputFixedMeshRes1 inputMovingMeshRes1 ";
    std::cerr << "outputResampledMeshRes1 ";
    std::cerr << "inputFixedMeshRes2 inputMovingMeshRes2 ";
    std::cerr << "outputResampledMeshRes2 ";
    std::cerr << "inputFixedMeshRes3 inputMovingMeshRes3 ";
    std::cerr << "outputResampledMeshRes3 ";
    std::cerr << "inputFixedMeshRes4 inputMovingMeshRes4 ";
    std::cerr << "outputResampledMeshRes4 ";
    std::cerr << "inputFixedMeshRes5 inputMovingMeshRes5 ";
    std::cerr << "outputResampledMeshRes5 ";
    std::cerr << "screenshotsTag ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  typedef float      MeshPixelType;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension >   MeshType;

  typedef itk::MultiResolutionQuadEdgeMeshSphericalDiffeomorphicDemonsFilter< 
    MeshType >  MultiResolutionDemonsFilterType;

  MultiResolutionDemonsFilterType::Pointer multiResDemonsFilter = MultiResolutionDemonsFilterType::New();


  typedef itk::VTKPolyDataReader< MeshType >     ReaderType;

  ReaderType::Pointer fixedMeshReader1 = ReaderType::New();
  fixedMeshReader1->SetFileName( argv[1] );

  ReaderType::Pointer movingMeshReader1 = ReaderType::New();
  movingMeshReader1->SetFileName( argv[2] );

  try
    {
    fixedMeshReader1->Update();
    movingMeshReader1->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  multiResDemonsFilter->SetFixedMesh( 0, fixedMeshReader1->GetOutput() );
  multiResDemonsFilter->SetMovingMesh( 0, movingMeshReader1->GetOutput() );


  ReaderType::Pointer fixedMeshReader2 = ReaderType::New();
  fixedMeshReader2->SetFileName( argv[3] );

  ReaderType::Pointer movingMeshReader2 = ReaderType::New();
  movingMeshReader2->SetFileName( argv[4] );

  try
    {
    fixedMeshReader2->Update();
    movingMeshReader2->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  multiResDemonsFilter->SetFixedMesh( 1, fixedMeshReader2->GetOutput() );
  multiResDemonsFilter->SetMovingMesh( 1, movingMeshReader2->GetOutput() );


  ReaderType::Pointer fixedMeshReader3 = ReaderType::New();
  fixedMeshReader3->SetFileName( argv[5] );

  ReaderType::Pointer movingMeshReader3 = ReaderType::New();
  movingMeshReader3->SetFileName( argv[6] );

  try
    {
    fixedMeshReader3->Update();
    movingMeshReader3->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  multiResDemonsFilter->SetFixedMesh( 2, fixedMeshReader3->GetOutput() );
  multiResDemonsFilter->SetMovingMesh( 2, movingMeshReader3->GetOutput() );



  ReaderType::Pointer fixedMeshReader4 = ReaderType::New();
  fixedMeshReader4->SetFileName( argv[7] );

  ReaderType::Pointer movingMeshReader4 = ReaderType::New();
  movingMeshReader4->SetFileName( argv[8] );

  try
    {
    fixedMeshReader4->Update();
    movingMeshReader4->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  multiResDemonsFilter->SetFixedMesh( 3, fixedMeshReader4->GetOutput() );
  multiResDemonsFilter->SetMovingMesh( 3, movingMeshReader4->GetOutput() );



  MultiResolutionDemonsFilterType::PointType center;
  center.Fill( 0.0 );

  const double radius = 100.0;

  multiResDemonsFilter->SetSphereCenter( center );
  multiResDemonsFilter->SetSphereRadius( radius );


  typedef MultiResolutionDemonsFilterType::DestinationPointSetType DestinationPointSetType;

#ifdef USE_VTK
  typedef DeformableAndAffineRegistrationMonitor< 
    DestinationPointSetType > RegistrationMonitorType;

  RegistrationMonitorType  visualMonitor;
  visualMonitor.SetNumberOfIterationsPerUpdate( 1 );

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 1");

  visualMonitor.SetVerbose( false );
  visualMonitor.SetScreenShotsBaseFileName( argv[16] );

  visualMonitor.SetScalarRange( -0.1, 0.1 );

  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader = 
    vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader->SetFileName( fixedMeshReader1->GetFileName() );
  vtkMovingMeshReader->SetFileName( movingMeshReader1->GetFileName() );

  vtkFixedMeshReader->Update();
  vtkMovingMeshReader->Update();

  visualMonitor.SetFixedSurface( vtkFixedMeshReader->GetOutput() );
  visualMonitor.SetMovingSurface( vtkMovingMeshReader->GetOutput() );

  multiResDemonsFilter->SetRegistrationMonitor( &visualMonitor );


#endif

  try
    {
    multiResDemonsFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
