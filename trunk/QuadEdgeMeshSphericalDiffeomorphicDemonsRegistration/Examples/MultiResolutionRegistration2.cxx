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
#include "MultiResolutionDeformableAndAffineRegistrationMonitor.h"
#include "vtkSmartPointer.h"
#include "vtkPolyDataReader.h"
#endif


int main( int argc, char * argv [] )
{

  if( argc < 13 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << std::endl;
    std::cerr << "inputFixedMeshRes1 inputMovingMeshRes1 ";
    std::cerr << "inputFixedMeshRes2 inputMovingMeshRes2 ";
    std::cerr << "inputFixedMeshRes3 inputMovingMeshRes3 ";
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

  multiResDemonsFilter->SetNumberOfResolutionLevels( 1 );

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
  typedef MultiResolutionDeformableAndAffineRegistrationMonitor< 
    DestinationPointSetType > RegistrationMonitorType;

  RegistrationMonitorType  visualMonitor;
  visualMonitor.SetNumberOfIterationsPerUpdate( 1 );

  visualMonitor.SetBaseAnnotationText("Rigid Registration Level 1");

  visualMonitor.SetVerbose( false );
  visualMonitor.SetScreenShotsBaseFileName( argv[13] );

  visualMonitor.SetScalarRange( -0.1, 0.1 );

  visualMonitor.SetNumberOfResolutionLevels( 4 );

  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader1 = vtkSmartPointer< vtkPolyDataReader >::New();
  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader1 = vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader1->SetFileName( fixedMeshReader1->GetFileName() );
  vtkMovingMeshReader1->SetFileName( movingMeshReader1->GetFileName() );

  vtkFixedMeshReader1->Update();
  vtkMovingMeshReader1->Update();

  visualMonitor.SetFixedSurface( 0, vtkFixedMeshReader1->GetOutput() );
  visualMonitor.SetMovingSurface( 0, vtkMovingMeshReader1->GetOutput() );


  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader2 = vtkSmartPointer< vtkPolyDataReader >::New();
  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader2 = vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader2->SetFileName( fixedMeshReader2->GetFileName() );
  vtkMovingMeshReader2->SetFileName( movingMeshReader2->GetFileName() );

  vtkFixedMeshReader2->Update();
  vtkMovingMeshReader2->Update();

  visualMonitor.SetFixedSurface( 1, vtkFixedMeshReader2->GetOutput() );
  visualMonitor.SetMovingSurface( 1, vtkMovingMeshReader2->GetOutput() );


  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader3 = vtkSmartPointer< vtkPolyDataReader >::New();
  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader3 = vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader3->SetFileName( fixedMeshReader3->GetFileName() );
  vtkMovingMeshReader3->SetFileName( movingMeshReader3->GetFileName() );

  vtkFixedMeshReader3->Update();
  vtkMovingMeshReader3->Update();

  visualMonitor.SetFixedSurface( 2, vtkFixedMeshReader3->GetOutput() );
  visualMonitor.SetMovingSurface( 2, vtkMovingMeshReader3->GetOutput() );


  vtkSmartPointer< vtkPolyDataReader > vtkFixedMeshReader4 = vtkSmartPointer< vtkPolyDataReader >::New();
  vtkSmartPointer< vtkPolyDataReader > vtkMovingMeshReader4 = vtkSmartPointer< vtkPolyDataReader >::New();

  vtkFixedMeshReader4->SetFileName( fixedMeshReader4->GetFileName() );
  vtkMovingMeshReader4->SetFileName( movingMeshReader4->GetFileName() );

  vtkFixedMeshReader4->Update();
  vtkMovingMeshReader4->Update();

  visualMonitor.SetFixedSurface( 3, vtkFixedMeshReader4->GetOutput() );
  visualMonitor.SetMovingSurface( 3, vtkMovingMeshReader4->GetOutput() );


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


  typedef itk::WarpQuadEdgeMeshFilter< 
    MeshType, MeshType, DestinationPointSetType >  WarpMeshFilterType;

  WarpMeshFilterType::Pointer warpFilter = WarpMeshFilterType::New();

  warpFilter->SetInput( fixedMeshReader4->GetOutput() );
  warpFilter->SetReferenceMesh( fixedMeshReader4->GetOutput() );

  warpFilter->SetDestinationPoints( multiResDemonsFilter->GetFinalDestinationPoints() );

  warpFilter->SetSphereRadius( radius );
  warpFilter->SetSphereCenter( center );


  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >   WriterType;
  WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( argv[9] );
  writer->SetInput( warpFilter->GetOutput() );

  try
    {
    warpFilter->Update();
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  ReaderType::Pointer fixedMeshReader5 = ReaderType::New();
  fixedMeshReader5->SetFileName( argv[10] );

  ReaderType::Pointer movingMeshReader5 = ReaderType::New();
  movingMeshReader5->SetFileName( argv[11] );

  try
    {
    fixedMeshReader5->Update();
    movingMeshReader5->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }


  warpFilter->SetInput( fixedMeshReader5->GetOutput() );
  writer->SetFileName( argv[12] );

  try
    {
    warpFilter->Update();
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
